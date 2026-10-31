# Stock assigns the shared user "android.uid.felica" (declared by
# MobileFeliCaClient.apk) the fixed UID 9000. The felica kernel
# driver only lets that UID (provisioned by mfsc) open the FeliCa
# devices, so the value must match the stock assignment.
[AID_FELICA]
value: 9000

[efs/]
mode: 0771
user: AID_SYSTEM
group: AID_RADIO
caps: 0

[firmware/]
mode: 0771
user: AID_SYSTEM
group: AID_SYSTEM
caps: 0

[firmware-modem/]
mode: 0771
user: AID_SYSTEM
group: AID_SYSTEM
caps: 0
