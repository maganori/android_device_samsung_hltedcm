/*
 * A5 <-> A11 binder bridge for the frozen oneseg libraries.
 *
 * Problem (see DESIGN-2026-11-22-binder-bridge.md and abi-check/NOTES.md):
 * sizeof(android::Parcel) grew from 48 bytes (Android 5.0) to 60 bytes
 * (Android 11). The frozen Bp proxies in libBroadcastProcObs.so and
 * libSDtvService.so place Parcels 48 bytes apart on their stack, so the
 * real Android 11 transact() writes the reply Parcel's tail fields
 * (mOwner/mOwnerCookie/mOpenAshmemSize at offsets 48..59) over the
 * proxy's saved registers and the caller crashes on return.
 *
 * This shim is interposed (TARGET_LD_SHIM_LIBS) into exactly three
 * frozen libraries: libbroadcastForOneSeg_jni.so,
 * libBroadcastProcObs.so and libSDtvService.so. It provides:
 *
 *  1. Parcel::Parcel() / Parcel::~Parcel() that only ever touch the
 *     first 48 bytes of the object, initialized with the Android 11
 *     field layout so the real libbinder Parcel methods (which the
 *     frozen code calls for everything else) work on them unchanged.
 *  2. defaultServiceManager() returning a ShimServiceManager that
 *     wraps the real service manager. Its getService()/checkService()
 *     wrap every returned binder in a ShimBinder.
 *  3. ShimBinder (a real Android 11 IBinder subclass; BBinder cannot
 *     be used because its transact() is final in A11) whose transact()
 *     converts the frozen 48-byte Parcels to real Parcels, calls the
 *     real BpBinder, and converts the reply back. The real transact()
 *     therefore never touches a frozen Parcel directly.
 *
 * Nothing in the frozen libraries is modified.
 *
 * Calling conventions verified in Phase 0 (abi-check/NOTES.md):
 *  - IServiceManager vtable slots used by the frozen code:
 *    +0x10 getService, +0x14 checkService, +0x18 addService. These
 *    match the Android 11 IServiceManager declaration order, so a
 *    C++ subclass of the A11 IServiceManager lands on the same slots.
 *  - IBinder vtable slots used on returned binders:
 *    +0x14 transact, +0x18 linkToDeath (queryLocalInterface goes
 *    through the libbinder symbol, not a raw slot).
 *  - The frozen code imports only Parcel C1/D1 (default ctor/dtor).
 */

#include <dlfcn.h>
#include <pthread.h>
#include <string.h>

#include <set>
#include <vector>

#include <binder/Binder.h>
#include <binder/BpBinder.h>
#include <binder/IBinder.h>
#include <binder/IServiceManager.h>
#include <binder/Parcel.h>
#include <log/log.h>
#include <utils/Mutex.h>
#include <utils/RefBase.h>
#include <utils/String16.h>
#include <utils/Vector.h>

namespace android {

// ---------------------------------------------------------------------------
// Real libbinder entry points, resolved through dlsym so that calls from
// inside this shim do not resolve back to the interposed definitions
// below (a shared library's own references bind to its own definitions).

typedef void (*ParcelCtorFn)(void* self);
typedef void (*ParcelDtorFn)(void* self);
typedef void (*DefaultServiceManagerFn)(void* sret);

static ParcelCtorFn gRealParcelCtor = nullptr;
static ParcelDtorFn gRealParcelDtor = nullptr;
static DefaultServiceManagerFn gRealDefaultServiceManager = nullptr;
static pthread_once_t gRealOnce = PTHREAD_ONCE_INIT;

static void resolveRealSymbols() {
    void* handle = dlopen("libbinder.so", RTLD_NOW | RTLD_NOLOAD);
    if (handle == nullptr) {
        // libbinder is a DT_NEEDED of every consumer of this shim, so
        // NOLOAD should never fail; fall back to a plain dlopen.
        handle = dlopen("libbinder.so", RTLD_NOW);
    }
    if (handle == nullptr) {
        ALOGE("oneseg_shim: cannot open libbinder.so: %s", dlerror());
        return;
    }
    gRealParcelCtor = reinterpret_cast<ParcelCtorFn>(
            dlsym(handle, "_ZN7android6ParcelC1Ev"));
    gRealParcelDtor = reinterpret_cast<ParcelDtorFn>(
            dlsym(handle, "_ZN7android6ParcelD1Ev"));
    gRealDefaultServiceManager = reinterpret_cast<DefaultServiceManagerFn>(
            dlsym(handle, "_ZN7android21defaultServiceManagerEv"));
    if (gRealParcelCtor == nullptr || gRealParcelDtor == nullptr ||
            gRealDefaultServiceManager == nullptr) {
        ALOGE("oneseg_shim: failed to resolve real libbinder symbols");
    }
}

static void ensureRealSymbols() {
    pthread_once(&gRealOnce, resolveRealSymbols);
}

// ---------------------------------------------------------------------------
// The first 48 bytes of an Android 11 Parcel. Offsets cross-checked
// against frameworks/native libs/binder/include/binder/Parcel.h
// (android-11.0.0_r1) and the Phase 0 notes: mOwner follows at 48.
// Used where member access is not available (transact conversion).

struct ParcelCore {
    status_t mError;                            // 0
    uint8_t* mData;                             // 4
    size_t mDataSize;                           // 8
    size_t mDataCapacity;                       // 12
    size_t mDataPos;                            // 16
    binder_size_t* mObjects;                    // 20
    size_t mObjectsSize;                        // 24
    size_t mObjectsCapacity;                    // 28
    size_t mNextObjectHint;                     // 32
    bool mObjectsSorted;                        // 36
    bool mRequestHeaderPresent;                 // 37
    uint8_t mPad1[2];                           // 38
    size_t mWorkSourceRequestHeaderPosition;    // 40
    bool mFdsKnown;                             // 44
    bool mHasFds;                               // 45
    bool mAllowFds;                             // 46
    uint8_t mPad2;                              // 47
};
static_assert(sizeof(ParcelCore) == 48, "ParcelCore must cover offsets 0..47");

static ParcelCore* coreOf(Parcel* parcel) {
    return reinterpret_cast<ParcelCore*>(parcel);
}

// A real, full-size (60-byte) Parcel whose lifetime is managed through
// the real ctor/dtor function pointers. Used for every Parcel this
// shim itself needs, so the interposed ctor below is never applied
// to shim-internal Parcels.
class RealParcel {
public:
    RealParcel() {
        ensureRealSymbols();
        gRealParcelCtor(mStorage);
    }
    ~RealParcel() {
        gRealParcelDtor(mStorage);
    }
    Parcel* get() {
        return reinterpret_cast<Parcel*>(mStorage);
    }
    const Parcel* get() const {
        return reinterpret_cast<const Parcel*>(mStorage);
    }
    Parcel* operator->() {
        return get();
    }

private:
    alignas(void*) uint8_t mStorage[sizeof(Parcel)];
};

// Release everything a frozen (48-byte-initialized) Parcel owns, by
// moving its core fields into a real Parcel shell and running the
// real destructor on the shell. The shell's tail (offsets 48..59)
// keeps the real ctor's values (mOwner == nullptr), so the real
// freeData path never dereferences anything from beyond the frozen
// buffer. The frozen Parcel is left empty.
static void releaseParcelContents(Parcel* parcel) {
    ParcelCore* core = coreOf(parcel);
    if (core->mData == nullptr && core->mObjects == nullptr) {
        return;
    }
    RealParcel shell;
    // Overwrite only the core; the shell tail stays real-initialized.
    memcpy(coreOf(shell.get()), core, sizeof(ParcelCore));
    core->mData = nullptr;
    core->mDataSize = 0;
    core->mDataCapacity = 0;
    core->mObjects = nullptr;
    core->mObjectsSize = 0;
    core->mObjectsCapacity = 0;
    // ~RealParcel() now releases the buffers and binder objects
    // exactly as the stock destructor would.
}

// ---------------------------------------------------------------------------
// Interposed Parcel ctor/dtor. These definitions replace the libbinder
// ones for calls originating in the three shimmed frozen libraries.
//
// The ctor initializes only offsets 0..47, with the *Android 11*
// values (see Parcel::initState in A11 Parcel.cpp), so that real A11
// Parcel methods called by the frozen code see a consistent object.
// Offsets 48..59 (mOwner/mOwnerCookie/mOpenAshmemSize) belong to the
// frozen caller's neighbouring stack slots and are never touched.

Parcel::Parcel() {
    mError = NO_ERROR;
    mData = nullptr;
    mDataSize = 0;
    mDataCapacity = 0;
    mDataPos = 0;
    mObjects = nullptr;
    mObjectsSize = 0;
    mObjectsCapacity = 0;
    mNextObjectHint = 0;
    mObjectsSorted = false;
    mRequestHeaderPresent = false;
    mWorkSourceRequestHeaderPosition = 0;
    mFdsKnown = true;
    mHasFds = false;
    mAllowFds = true;
}

Parcel::~Parcel() {
    releaseParcelContents(this);
}

// ---------------------------------------------------------------------------
// ShimDeathRecipient: receives death notifications from the real
// BpBinder and forwards them to the frozen A5 DeathRecipient.
//
// The frozen recipient's binderDied() lives at vtable offset +0x18 of
// its DeathRecipient subobject (RefBase virtuals occupy +0x00..+0x14
// in both generations; layout verified in Phase 0). The wp<IBinder>
// handed over refers to the real binder, which is also what the
// frozen code would have observed without this shim.

class ShimDeathRecipient : public IBinder::DeathRecipient {
public:
    ShimDeathRecipient(IBinder::DeathRecipient* frozenRecipient)
        : mFrozen(frozenRecipient) {
    }

    void binderDied(const wp<IBinder>& who) override {
        void** vtable = *reinterpret_cast<void***>(mFrozen);
        typedef void (*BinderDiedFn)(void* self, const wp<IBinder>& who);
        reinterpret_cast<BinderDiedFn>(vtable[6])(mFrozen, who);
    }

    IBinder::DeathRecipient* frozen() const {
        return mFrozen;
    }

private:
    IBinder::DeathRecipient* mFrozen;
};

// ---------------------------------------------------------------------------
// ShimBinder: wraps a real BpBinder obtained from the service manager.
//
// It derives directly from IBinder (BBinder::transact is final in
// Android 11, so a BBinder subclass cannot intercept transact()).
// Its primary vtable is therefore the IBinder interface vtable, with
// the same slot layout the frozen code was compiled against
// (+0x14 transact, +0x18 linkToDeath). queryLocalInterface() returns
// nullptr for every real descriptor (forwarded to the real binder,
// which is remote), so the frozen asInterface() builds its own Bp
// proxy with this object as the remote, exactly as designed.

class ShimBinder : public IBinder {
public:
    explicit ShimBinder(const sp<IBinder>& real)
        : mReal(real) {
        Mutex::Autolock lock(registryLock());
        registry().insert(this);
    }

    ~ShimBinder() override {
        Mutex::Autolock lock(registryLock());
        registry().erase(this);
    }

    // Registry of live wrappers, so wrapBinder() can recognize a
    // binder that is already one of ours.
    static std::set<ShimBinder*>& registry() {
        static std::set<ShimBinder*> sRegistry;
        return sRegistry;
    }
    static Mutex& registryLock() {
        static Mutex sLock;
        return sLock;
    }
    static bool isShimBinder(IBinder* binder) {
        Mutex::Autolock lock(registryLock());
        return registry().count(reinterpret_cast<ShimBinder*>(binder)) != 0;
    }

    sp<IInterface> queryLocalInterface(const String16& descriptor) override {
        // The real binder is remote, so this returns nullptr and the
        // frozen asInterface() builds its own Bp proxy over us.
        return mReal->queryLocalInterface(descriptor);
    }

    const String16& getInterfaceDescriptor() const override {
        return mReal->getInterfaceDescriptor();
    }

    bool isBinderAlive() const override {
        return mReal->isBinderAlive();
    }

    status_t pingBinder() override {
        return mReal->pingBinder();
    }

    status_t dump(int fd, const Vector<String16>& args) override {
        return mReal->dump(fd, args);
    }

    // Present ourselves as a remote binder: if frozen code ever writes
    // this wrapper into a Parcel via writeStrongBinder(), flattenBinder
    // must produce the real binder's handle, not a local reference to
    // the wrapper.
    BBinder* localBinder() override {
        return nullptr;
    }
    BpBinder* remoteBinder() override {
        return static_cast<BpBinder*>(mReal.get());
    }

    status_t transact(uint32_t code, const Parcel& data, Parcel* reply,
            uint32_t flags = 0) override {
        // Data: the frozen Parcel is a valid read source for
        // appendFrom (it only reads core fields, offsets < 48).
        RealParcel realData;
        status_t err = realData->appendFrom(&data, 0, data.dataSize());
        if (err != NO_ERROR) {
            ALOGE("oneseg_shim: transact %u: data appendFrom failed %d",
                    code, err);
            return err;
        }

        RealParcel realReply;
        Parcel* realReplyPtr = (reply != nullptr) ? realReply.get() : nullptr;
        err = mReal->transact(code, *realData.get(), realReplyPtr, flags);
        if (err != NO_ERROR || reply == nullptr) {
            return err;
        }

        // Reply: the real reply Parcel's buffer is owned by
        // IPCThreadState (mOwner is set), so it cannot be handed to
        // the frozen Parcel directly. Deep-copy it into a temporary
        // real Parcel (appendFrom acquires its own references and
        // dups fds), then steal the temporary's buffers into the
        // frozen reply Parcel's core fields. Stealing writes only
        // offsets 0..47 of the frozen Parcel.
        RealParcel tmp;
        err = tmp->appendFrom(realReply.get(), 0, realReply->dataSize());
        if (err != NO_ERROR) {
            ALOGE("oneseg_shim: transact %u: reply appendFrom failed %d",
                    code, err);
            return err;
        }
        releaseParcelContents(reply);
        ParcelCore* dst = coreOf(reply);
        ParcelCore* src = coreOf(tmp.get());
        *dst = *src;
        dst->mDataPos = 0;
        // The temporary must not free what the frozen Parcel now owns.
        src->mData = nullptr;
        src->mDataSize = 0;
        src->mDataCapacity = 0;
        src->mObjects = nullptr;
        src->mObjectsSize = 0;
        src->mObjectsCapacity = 0;
        return NO_ERROR;
    }

    status_t linkToDeath(const sp<DeathRecipient>& recipient,
            void* cookie = nullptr, uint32_t flags = 0) override {
        DeathRecipient* frozen = recipient.get();
        sp<ShimDeathRecipient> shimRecipient = new ShimDeathRecipient(frozen);
        status_t err = mReal->linkToDeath(shimRecipient, cookie, flags);
        if (err != NO_ERROR) {
            return err;
        }
        // Keep the frozen recipient alive for as long as the link
        // exists; released in unlinkToDeath().
        frozen->incStrong(frozen);
        DeathLink link;
        link.frozen = frozen;
        link.cookie = cookie;
        link.shim = shimRecipient;
        Mutex::Autolock lock(mLinksLock);
        mLinks.push_back(link);
        return NO_ERROR;
    }

    status_t unlinkToDeath(const wp<DeathRecipient>& recipient,
            void* cookie = nullptr, uint32_t flags = 0,
            wp<DeathRecipient>* outRecipient = nullptr) override {
        (void)outRecipient;
        DeathRecipient* frozen = nullptr;
        sp<ShimDeathRecipient> shimRecipient;
        {
            Mutex::Autolock lock(mLinksLock);
            // wp::unsafe_get() is not available in A11 headers for all
            // instantiations; compare through the stored entries by
            // promoting is not possible either (the wp may already be
            // dead). Match on cookie plus the promoted pointer when
            // promotion succeeds.
            sp<DeathRecipient> promoted = recipient.promote();
            for (size_t i = 0; i < mLinks.size(); i++) {
                if (mLinks[i].cookie == cookie &&
                        (promoted == nullptr ||
                         mLinks[i].frozen == promoted.get())) {
                    frozen = mLinks[i].frozen;
                    shimRecipient = mLinks[i].shim;
                    mLinks.erase(mLinks.begin() + i);
                    break;
                }
            }
        }
        if (shimRecipient == nullptr) {
            // Not a link we created; forward a best-effort unlink with
            // the original recipient (should not happen in practice).
            return mReal->unlinkToDeath(recipient, cookie, flags);
        }
        status_t err = mReal->unlinkToDeath(
                wp<DeathRecipient>(shimRecipient), cookie, flags);
        frozen->decStrong(frozen);
        return err;
    }

private:
    struct DeathLink {
        DeathRecipient* frozen;
        void* cookie;
        sp<ShimDeathRecipient> shim;
    };

    sp<IBinder> mReal;
    Mutex mLinksLock;
    std::vector<DeathLink> mLinks;
};

// Wrap a real binder in a ShimBinder (idempotent: binders that are
// already ours are returned unchanged).
static sp<IBinder> wrapBinder(const sp<IBinder>& real) {
    if (real == nullptr) {
        return nullptr;
    }
    if (ShimBinder::isShimBinder(real.get())) {
        return real;
    }
    return sp<IBinder>(new ShimBinder(real));
}

// ---------------------------------------------------------------------------
// ShimServiceManager: wraps the real IServiceManager. Slot order of
// the A11 IServiceManager vtable (getService +0x10, checkService
// +0x14, addService +0x18 relative to the interface address point)
// matches what the frozen code calls (Phase 0).

class ShimServiceManager : public IServiceManager {
public:
    explicit ShimServiceManager(const sp<IServiceManager>& real)
        : mReal(real) {
    }

    sp<IBinder> getService(const String16& name) const override {
        return wrapBinder(mReal->getService(name));
    }

    sp<IBinder> checkService(const String16& name) const override {
        return wrapBinder(mReal->checkService(name));
    }

    status_t addService(const String16& name, const sp<IBinder>& service,
            bool allowIsolated = false,
            int dumpsysFlags = DUMP_FLAG_PRIORITY_DEFAULT) override {
        // The frozen caller passes only the first three arguments
        // (the A5 signature); dumpsysFlags is read from a stack slot
        // it never set, so always substitute the default.
        (void)dumpsysFlags;
        return mReal->addService(name, service, allowIsolated,
                DUMP_FLAG_PRIORITY_DEFAULT);
    }

    Vector<String16> listServices(int dumpsysFlags) override {
        return mReal->listServices(dumpsysFlags);
    }

    sp<IBinder> waitForService(const String16& name) override {
        return wrapBinder(mReal->waitForService(name));
    }

    bool isDeclared(const String16& name) override {
        return mReal->isDeclared(name);
    }

protected:
    IBinder* onAsBinder() override {
        // A11 IInterface::asBinder is a static helper; applied to the
        // real service manager it yields the real SM binder.
        return IInterface::asBinder(mReal.get()).get();
    }

private:
    sp<IServiceManager> mReal;
};

// ---------------------------------------------------------------------------
// Interposed defaultServiceManager(). Returns a process-wide
// ShimServiceManager singleton wrapping the real service manager.

static pthread_mutex_t gShimSmLock = PTHREAD_MUTEX_INITIALIZER;
static sp<ShimServiceManager> gShimSm;

sp<IServiceManager> defaultServiceManager() {
    ensureRealSymbols();
    if (gRealDefaultServiceManager == nullptr) {
        return nullptr;
    }
    pthread_mutex_lock(&gShimSmLock);
    if (gShimSm == nullptr) {
        sp<IServiceManager> real;
        // The real function constructs the returned sp in the sret
        // buffer; sp<IServiceManager> is a single pointer, so calling
        // it with &real is exact.
        gRealDefaultServiceManager(&real);
        if (real != nullptr) {
            gShimSm = new ShimServiceManager(real);
            ALOGI("oneseg_shim: service manager bridge installed");
        }
    }
    sp<ShimServiceManager> result = gShimSm;
    pthread_mutex_unlock(&gShimSmLock);
    if (result == nullptr) {
        return nullptr;
    }
    return sp<IServiceManager>(result.get());
}

} // namespace android
