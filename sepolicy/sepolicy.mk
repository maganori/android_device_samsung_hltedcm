#
# Copyright (C) 2018 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Board specific SELinux policy variable definitions
BOARD_SEPOLICY_DIRS += \
    device/samsung/hltedcm/sepolicy/common

# FeliCa (from lineage-15.1; see the porting notes in the commit message -
# in the split sepolicy of 18.1 the /system/bin file_contexts entries and
# the platform-domain rules in this directory are the expected rework point)
BOARD_SEPOLICY_DIRS += \
    device/samsung/hltedcm/sepolicy/felica

# Oneseg (from lineage-15.1-oneseg)
BOARD_SEPOLICY_DIRS += \
    device/samsung/hltedcm/sepolicy/oneseg
