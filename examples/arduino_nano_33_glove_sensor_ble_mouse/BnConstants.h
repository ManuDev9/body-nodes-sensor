/**
* MIT License
* 
* Copyright (c) 2019-2024 Manuel Bottini
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifndef __BN_CONSTANTS_H
#define __BN_CONSTANTS_H

#define PLAYER_ALL_TAG      "all"
#define PLAYER_NONE_TAG     "none"

// BODY PARTS NAMES
#define BODYPART_HEAD_TAG             "head"
#define BODYPART_HAND_LEFT_TAG        "hand_left"
#define BODYPART_LOWERARM_LEFT_TAG    "lowerarm_left"
#define BODYPART_UPPERARM_LEFT_TAG    "upperarm_left"
#define BODYPART_BODY_TAG             "body"
#define BODYPART_LOWERARM_RIGHT_TAG   "lowerarm_right"
#define BODYPART_UPPERARM_RIGHT_TAG   "upperarm_right"
#define BODYPART_HAND_RIGHT_TAG       "hand_right"
#define BODYPART_LOWERLEG_LEFT_TAG    "lowerleg_left"
#define BODYPART_UPPERLEG_LEFT_TAG    "upperleg_left"
#define BODYPART_FOOT_LEFT_TAG        "foot_left"
#define BODYPART_LOWERLEG_RIGHT_TAG   "lowerleg_right"
#define BODYPART_UPPERLEG_RIGHT_TAG   "upperleg_right"
#define BODYPART_FOOT_RIGHT_TAG       "foot_right"
#define BODYPART_UPPERBODY_TAG        "upperbody"
#define BODYPART_LOWERBODY_TAG        "lowerbody"
#define BODYPART_KATANA_TAG           "katana"
#define BODYPART_UNTAGGED_TAG         "untagged"
#define BODYPART_ALL_TAG              "all"

// BODY PARTS HEX CODES (Important for the preprocessing macros)
#define BODYPART_HEAD_HEX             0x0000
#define BODYPART_HAND_LEFT_HEX        0x0001
#define BODYPART_LOWERARM_LEFT_HEX    0x0002
#define BODYPART_UPPERARM_LEFT_HEX    0x0003
#define BODYPART_BODY_HEX             0x0004
#define BODYPART_LOWERARM_RIGHT_HEX   0x0005
#define BODYPART_UPPERARM_RIGHT_HEX   0x0006
#define BODYPART_HAND_RIGHT_HEX       0x0007
#define BODYPART_LOWERLEG_LEFT_HEX    0x0008
#define BODYPART_UPPERLEG_LEFT_HEX    0x0009
#define BODYPART_FOOT_LEFT_HEX        0x000A
#define BODYPART_LOWERLEG_RIGHT_HEX   0x000B
#define BODYPART_UPPERLEG_RIGHT_HEX   0x000C
#define BODYPART_FOOT_RIGHT_HEX       0x000D
#define BODYPART_UPPERBODY_HEX        0x000E
#define BODYPART_LOWERBODY_HEX        0x000F
#define BODYPART_KATANA_HEX           0x0010
#define BODYPART_UNTAGGED_HEX         0x0011
#define BODYPART_ALL_HEX              0xFFFE // Used in some particular cases

// ACTIONS
#define ACTION_TYPE_TAG               "type"
#define ACTION_PLAYER_TAG             "player"
#define ACTION_BODYPART_TAG           "bodypart"

#define ACTION_TYPE_NONE_TAG          "none"
#define ACTION_TYPE_HAPTIC_TAG        "haptic"
#define ACTION_TYPE_SETPLAYER_TAG     "set_player"
#define ACTION_TYPE_SETBODYPART_TAG   "set_bodypart"
#define ACTION_TYPE_ENABLESENSOR_TAG  "enable_sensor"
#define ACTION_TYPE_SETWIFI_TAG       "set_wifi"

#define ACTION_HAPTIC_DURATION_MS_TAG       "duration_ms"
#define ACTION_HAPTIC_STRENGTH_TAG          "strength"
#define ACTION_SETPLAYER_NEWPLAYER_TAG      "new_player"
#define ACTION_SETBODYPART_NEWBODYPART_TAG  "new_bodypart"
#define ACTION_ENABLESENSOR_SENSORTYPE_TAG  "sensortype"
#define ACTION_ENABLESENSOR_ENABLE_TAG      "enable"
#define ACTION_SETWIFI_SSID_TAG             "ssid"
#define ACTION_SETWIFI_PASSWORD_TAG         "password"
#define ACTION_SETWIFI_MULTICASTMESSAGE_TAG "multicast_message"

// MESSAGE
#define MESSAGE_PLAYER_TAG      "player"
#define MESSAGE_BODYPART_TAG    "bodypart"
#define MESSAGE_SENSORTYPE_TAG  "sensortype"
#define MESSAGE_VALUE_TAG       "value"
#define MESSAGE_VALUE_RESET_TAG "reset"

// MEMORY
#define MEMORY_BODYPART_TAG               "bodypart"
#define MEMORY_BODYPART_GLOVE_TAG         "bodypart_glove"
#define MEMORY_BODYPART_SHOE_TAG          "bodypart_shoe"
#define MEMORY_PLAYER_TAG                 "player"
#define MEMORY_WIFI_SSID_TAG              "wifi_ssid"
#define MEMORY_WIFI_PASSWORD_TAG          "wifi_password"
#define MEMORY_WIFI_MULTICASTMESSAGE_TAG  "multicast_message"

// SENSORTYPES 
#define SENSORTYPE_NONE_TAG               "none"
#define SENSORTYPE_ORIENTATION_ABS_TAG    "orientation_abs"
#define SENSORTYPE_ACCELERATION_REL_TAG   "acceleration_rel"
#define SENSORTYPE_GLOVE_TAG              "glove"
#define SENSORTYPE_SHOE_TAG               "shoe"

// SENSOR STATUS
#define SENSOR_STATUS_NOT_ACCESSIBLE  1
#define SENSOR_STATUS_CALIBRATING     2
#define SENSOR_STATUS_WORKING         3

// CONNECTION STATUS
#define CONNECTION_STATUS_NOT_CONNECTED  0
#define CONNECTION_STATUS_WAITING_ACK    1
#define CONNECTION_STATUS_CONNECTED      2

// Wifi Connections
#define BODYNODES_PORT 12345
#define BODYNODES_MULTICAST_PORT 12346
#define BODYNODES_WIFI_SSID_DEFAULT "BodynodeHotspot"
#define BODYNODES_WIFI_PASS_DEFAULT "bodynodes1"
#define BODYNODES_MULTICASTGROUP_DEFAULT  "239.192.1.99"
#define BODYNODES_MULTICASTMESSAGE_DEFAULT  "BN"

// Bluetooth Connections
//TODO

// BLE Connections
#define BLE_BODYNODES_NAME                                      "Bodynode"
#define BLE_BODYNODES_SERVICE_UUID                              "0000CCA0-0000-1000-8000-00805F9B34FB"
#define BLE_BODYNODES_CHARA_PLAYER_UUID                         "0000CCA1-0000-1000-8000-00805F9B34FB"
#define BLE_BODYNODES_CHARA_BODYPART_UUID                       "0000CCA2-0000-1000-8000-00805F9B34FB"
#define BLE_BODYNODES_CHARA_ORIENTATION_ABS_VALUE_UUID          "0000CCA3-0000-1000-8000-00805F9B34FB"
#define BLE_BODYNODES_CHARA_ACCELERATION_REL_VALUE_UUID         "0000CCA4-0000-1000-8000-00805F9B34FB"
#define BLE_BODYNODES_CHARA_GLOVE_VALUE_UUID                    "0000CCA5-0000-1000-8000-00805F9B34FB"
#define BLE_BODYNODES_CHARA_SHOE_UUID                           "0000CCA6-0000-1000-8000-00805F9B34FB"


#endif //__BN_CONSTANTS_H
