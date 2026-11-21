#!/system/bin/sh
# Unlock the FeliCa CEN gate, with retries.
#
# The CEN value is held by the SE lock chip and reached over I2C from
# the felica driver. A single write at post-fs-data does not latch:
# at that point the write fails (the FeliCa side is not ready yet) and
# the driver keeps reporting the locked state. The NFC stack reads
# CEN when it initializes the FeliCa library, so the unlock has to be
# in place before that, without knowing exactly when the chip starts
# answering.
#
# Write the unlock byte, then read the value back; the driver latches
# the unlocked state after a successful write, so a read-back of 1
# means we are done. Retry once a second, bounded to about 90 tries,
# so a missing or broken chip cannot loop forever.
n=0
while [ $n -lt 90 ]; do
    if [ -e /dev/felica_cen ]; then
        printf '\001' > /dev/felica_cen
        # dd + od print the byte value as decimal text (with spaces);
        # the unquoted echo below collapses the whitespace.
        v=$(dd if=/dev/felica_cen bs=1 count=1 2>/dev/null | od -An -tu1)
        v=$(echo $v)
        if [ "$v" = "1" ]; then
            exit 0
        fi
    fi
    n=$((n + 1))
    sleep 1
done
exit 0
