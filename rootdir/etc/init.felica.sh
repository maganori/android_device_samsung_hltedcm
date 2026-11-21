#!/system/bin/sh
if [ -r /data/media/0/kbc/cmdline ]; then
    cat /data/media/0/kbc/cmdline > /proc/cmdline
elif [ -r /data/media/0/kbc/felica_key ]; then
    local ORIG_CMDLINE=`cat /proc/cmdline`
    local FELICA_KEY=`head -1 /data/media/0/kbc/felica_key`
    set -- $FELICA_KEY
    local CORDON="cordon=$1"
    local CONNIE="connie=$2"
    echo -n "$CORDON $CONNIE $ORIG_CMDLINE" | sed -e 's/^ +//' > /proc/cmdline
fi

# Unlock the FeliCa CEN gate. The value held by the felica driver is
# volatile and resets to 0 on every boot. The NFC stack reads CEN when
# it initializes the FeliCa library; with CEN=0 it never starts reader
# discovery for NFC-F tags, so write the unlock byte here, before the
# NFC service comes up. The write is done as root from this script.
if [ -e /dev/felica_cen ]; then
    printf '\001' > /dev/felica_cen
fi
exit 0
