Copyright 2017-2019 - The LineageOS Project
Copyright 2015-2016 - The CyanogenMod Project

Device configuration for Samsung Galaxy Note 3 (SC-01F).

WORK IN PROGRESS. WILL EAT YOUR CAT.
------------------------------------------------------------------

  - For building LineageOS 17.0, create '.repo/local_manifests/roomservice.xml'
    with the following content:

<?xml version="1.0" encoding="UTF-8"?>
  <manifest>

    <project name="LineageOS/android_hardware_samsung" path="hardware/samsung" remote="github" revision="lineage-17.0" />
    <project name="LineageOS/android_packages_resources_devicesettings" path="packages/resources/devicesettings" remote="github" revision="lineage-17.0" />
    <project name="LineageOS/android_packages_apps_FlipFlap" path="packages/apps/FlipFlap" remote="github" revision="lineage-17.0" />

    <project name="LineageOS/android_device_samsung_qcom-common" path="device/samsung/qcom-common" remote="github" revision="lineage-17.0" />
    <project name="mohammad92/android_device_samsung_msm8974-common" path="device/samsung/msm8974-common" remote="github" revision="lineage-17.0" />

    <project name="kyasu/android_kernel_samsung_msm8974" path="kernel/samsung/msm8974" remote="github" revision="lineage-17.0-test" />
    <project name="maganori/android_device_samsung_hltedcm" path="device/samsung/hltedcm" remote="github" revision="lineage-17.0-test" />
    <project name="kyasu/android_vendor_samsung_hltedcm" path="vendor/samsung/hltedcm" remote="github" revision="lineage-16.0" />

  </manifest>
