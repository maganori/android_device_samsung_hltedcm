/*
 * Copyright (C) 2017 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ui/GraphicBuffer.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
{
#endif
/*
android::GraphicBuffer::GraphicBuffer(native_handle const*, android::GraphicBuffer::HandleWrapMethod, unsigned int, unsigned int, int, unsigned int, unsigned long long, unsigned int)
*/
extern void _ZN7android13GraphicBufferC1EPK13native_handleNS0_16HandleWrapMethodEjjijyj(
	void *self, const native_handle_t* handle,
        android::GraphicBuffer::HandleWrapMethod method, 
        uint32_t width, uint32_t height, android::PixelFormat format, 
        uint32_t layerCount, uint64_t usage, uint32_t stride);

/*
android::GraphicBuffer::GraphicBuffer(ANativeWindowBuffer*, bool)
*/
void _ZN7android13GraphicBufferC1EP19ANativeWindowBufferb(
	void *self, ANativeWindowBuffer* buffer, bool keepOwnership)
{
    _ZN7android13GraphicBufferC1EPK13native_handleNS0_16HandleWrapMethodEjjijyj(
	self, buffer->handle, 
	keepOwnership ? android::GraphicBuffer::TAKE_HANDLE : android::GraphicBuffer::WRAP_HANDLE,
	buffer->width, buffer->height, buffer->format, buffer->layerCount, 
	buffer->usage, buffer->stride);
}

/*
android::GraphicBuffer::lock(unsigned int, void**, int*, int*) - Android 11.
The two int outputs (the locked area's dimensions) are optional;
the legacy two-argument lock() below passes scratch variables.
*/
extern int _ZN7android13GraphicBuffer4lockEjPPvPiS3_(
	void *self, uint32_t usage, void** vaddr, int32_t* outWidth, int32_t* outHeight);

/*
android::GraphicBuffer::lock(unsigned int, void**) - legacy signature
imported by libMmbSfDplMp.so; removed from libui in Android 11.
*/
int _ZN7android13GraphicBuffer4lockEjPPv(
	void *self, uint32_t usage, void** vaddr)
{
    int32_t width = 0;
    int32_t height = 0;
    return _ZN7android13GraphicBuffer4lockEjPPvPiS3_(self, usage, vaddr, &width, &height);
}

/*
 * android::Fence::~Fence() - imported by libMmbMfMp4FwMw.so.
 *
 * Android 11 defines the destructor inline in <ui/Fence.h>, so
 * libui exports no D1/D2 symbol. In both the legacy and the
 * Android 11 layout the object is a LightRefBase (one 32-bit
 * refcount, no vtable) followed by the fence fd as a plain int,
 * and the objects the blob destroys were created by Android 11's
 * own Fence constructor, so the fd sits at offset 4. Close it the
 * way the real destructor does; fds 0-2 are never fence fds and
 * NO_FENCE is -1.
 */
void _ZN7android5FenceD1Ev(void *self)
{
    int *fd = reinterpret_cast<int*>(reinterpret_cast<char*>(self) + 4);
    if (*fd > 2) {
        close(*fd);
        *fd = -1;
    }
}

void _ZN7android5FenceD2Ev(void *self)
{
    _ZN7android5FenceD1Ev(self);
}

#ifdef __cplusplus
}
#endif
