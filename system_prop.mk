# Audio
PRODUCT_PROPERTY_OVERRIDES += \
    persist.audio.fluence.speaker=true \
    persist.audio.fluence.voicecall=true \
    persist.audio.fluence.voicerec=false \
    ro.qc.sdk.audio.fluencetype=fluence \
    mm.enable.smoothstreaming=true \
    av.streaming.offload.enable=true \
    use.voice.path.for.pcm.voip=true \
    audio.offload.multiple.enabled=true \
    audio.offload.gapless.enabled=true \
    tunnel.audio.encode=true \
    media.aac_51_output_enabled=true \
    audio.offload.pcm.16bit.enable=true \
    audio.offload.pcm.24bit.enable=true

# Camera
PRODUCT_PROPERTY_OVERRIDES += \
    camera2.portability.force_api=1

# Display
PRODUCT_PROPERTY_OVERRIDES += \
    ro.hdcp2.rx=tz \
    ro.qualcomm.cabl=1 \
    ro.secwvk=144 \
    ro.sf.lcd_density=480

# GPS
PRODUCT_PROPERTY_OVERRIDES += \
    persist.gps.qc_nlp_in_use=0 \
    ro.gps.agps_provider=1 \
    ro.qc.sdk.izat.premium_enabled=0 \
    ro.qc.sdk.izat.service_mask=0x0

# perf api
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.extension_library=/vendor/lib/libqti-perfd-client.so

# NFC
PRODUCT_PROPERTY_OVERRIDES += \
   ro.nfc.port=I2C

# Radio
PRODUCT_PROPERTY_OVERRIDES += \
    persist.timed.enable=true \
    persist.radio.apm_sim_not_pwdn=1 \
    persist.radio.no_wait_for_card=1 \
    persist.radio.add_power_save=1 \
    persist.eons.enabled=false \
    persist.radio.use_se_table_only=1 \
    ro.use_data_netmgrd=false \
    persist.data.netmgrd.qos.enable=false \
    persist.telephony.mpdn=false \
    persist.radio.dcmlte=true \
    persist.radio.prevmcc=440 \
    persist.radio.setnwkmode=9 \
    persist.radio.forced_succ_ack=false \
    persist.radio.default_wappush=true \
    ro.telephony.default_network=0 \
    telephony.lteOnCdmaDevice=0 \
    ro.telephony.ril_class=hltedcmRIL

# Sensors
PRODUCT_PROPERTY_OVERRIDES += \
    debug.sensors=1

# Wifi
PRODUCT_PROPERTY_OVERRIDES += \
    wifi.interface=wlan0