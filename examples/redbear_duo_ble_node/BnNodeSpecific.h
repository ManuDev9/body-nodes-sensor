/**
* MIT License
*
* Copyright (c) 2021-2025 Manuel Bottini
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

// General
#include <Arduino.h> 
#include <ArduinoJson.h>

#include "BnConstants.h"

// Implements Specification Version Dev 1.0
// Sensortypes: orientation_abs, glove
// Board: RedBear Duo (Native USB Port)

#ifndef __BN_NODE_SPECIFIC_H
#define __BN_NODE_SPECIFIC_H

#define BODYNODE_BODYPART_HEX_DEFAULT BODYPART_UPPERARM_RIGHT_HEX
#define BODYNODE_PLAYER_TAG_DEFAULT  "1"

// COMMUNICATION //
#define BLE_COMMUNICATION

// SENSORS //
#define ORIENTATION_ABS_SENSOR

// ACTUATORS //

// Please remember to define the following as wanted to tag the bodypart on data related to GLOVE and/or SHOE sensors
#define BODYNODE_BODYPART_GLOVE_TAG BODYPART_HAND_RIGHT_TAG
#define BODYNODE_BODYPART_SHOE_TAG BODYPART_FOOT_RIGHT_TAG


#define SENSOR_READ_INTERVAL_MS 30
#define BIG_QUAT_DIFF 0.002
#define BIG_ANGLE_DIFF 6
#define CONNECTION_ACK_INTERVAL_MS 1000
#define CONNECTION_KEEP_ALIVE_SEND_INTERVAL_MS 30000
#define CONNECTION_KEEP_ALIVE_REC_INTERVAL_MS 60000
#define MULTICAST_KEEP_ALIVE_REC_INTERVAL_MS 30000

// Device Specific Axis Configuration
// Use the program "sensor_test" and check the "bodynodes universal orientation specs" to build your axis configuration,
// so that your node can easily integrate with any system.

// The BNO055 sure is weird with these orientations by default
#define OUT_AXIS_W 2
#define OUT_AXIS_X 0
#define OUT_AXIS_Y 3
#define OUT_AXIS_Z 1

#define MUL_AXIS_W 1
#define MUL_AXIS_X -1
#define MUL_AXIS_Y -1
#define MUL_AXIS_Z -1

// PINS
#define BUZZER_FREQ 1000 //Specified in Hz
#define LED_DT_ON 1 // Duty cicle of LED ON

#define STATUS_SENSOR_HMI_LED_P      6
#define STATUS_SENSOR_HMI_LED_M      5
#define STATUS_CONNECTION_HMI_LED_P  3
#define STATUS_CONNECTION_HMI_LED_M  2
#define HAPTIC_MOTOR_PIN_P           10
#define HAPTIC_MOTOR_PIN_M           11
#define SHOE_SENSOR_PIN_P            19

#define MAX_BUFF_LENGTH 100

#define DEBUG_M
#ifdef DEBUG_M
 #define DEBUG_PRINT(x)  Serial.print (x)
 #define DEBUG_PRINT_HEX(x)  Serial.print (x,HEX)
 #define DEBUG_PRINT_DEC(x)  Serial.print (x,DEC)
 #define DEBUG_PRINTLN(x)  Serial.println (x)
 #define DEBUG_PRINTLN_HEX(x)  Serial.println (x,HEX)
 #define DEBUG_PRINTLN_DEC(x)  Serial.println (x,DEC)
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINT_HEX(x)
 #define DEBUG_PRINT_DEC(x)
 #define DEBUG_PRINTLN(x)
 #define DEBUG_PRINTLN_HEX(x)
 #define DEBUG_PRINTLN_DEC(x)
#endif

#define BODYNODES_PORT 12345
#define BODYNODES_MULTICAST_PORT 12346
#define BODYNODES_WIFI_SSID_DEFAULT "BodynodeHotspot"
#define BODYNODES_WIFI_PASS_DEFAULT "bodynodes1"
#define BODYNODES_MULTICASTGROUP_DEFAULT  "239.192.1.99"
#define BODYNODES_MULTICASTMESSAGE_DEFAULT  "BN"

// Set BODYNODE_BODYPART_TAG_DEFAULT
#if BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_HEAD_HEX
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_HAND_LEFT_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_HAND_LEFT_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_LOWERARM_LEFT_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_LOWERARM_LEFT_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_UPPERARM_LEFT_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_UPPERARM_LEFT_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_BODY_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_BODY_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_LOWERARM_RIGHT_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_LOWERARM_RIGHT_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_UPPERARM_RIGHT_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_UPPERARM_RIGHT_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_HAND_RIGHT_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_HAND_RIGHT_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_LOWERLEG_LEFT_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_LOWERLEG_LEFT_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_UPPERLEG_LEFT_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_UPPERLEG_LEFT_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_FOOT_LEFT_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_FOOT_LEFT_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_LOWERLEG_RIGHT_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_LOWERLEG_RIGHT_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_UPPERLEG_RIGHT_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_UPPERLEG_RIGHT_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_FOOT_RIGHT_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_FOOT_RIGHT_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_UPPERBODY_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_UPPERBODY_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_LOWERBODY_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_LOWERBODY_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_KATANA_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_KATANA_TAG
#elif BODYNODE_BODYPART_HEX_DEFAULT == BODYPART_UNTAGGED_HEX
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_UNTAGGED_TAG
#endif // BODYNODE_BODYPART_TAG_DEFAULT

// Node Specific functions definitions
// Defines have been chose because we want code to be easily place in the functions
// So that we don't have to deal in passing weird datatypes that might differ depending
// on the platform.
// In order to debug, just take the content and put it directly on the funtion itself

#if defined(ARDUINO)
  #define BN_NODE_SPECIFIC_MAIN_FILE_INIT SYSTEM_MODE(MANUAL);
#endif

#define BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_WRITE_STATUS_PIN_FUNCTION analogWrite

// Other node specific utility functions that are defined in the same way
void persMemoryInit();
void persMemoryCommit();
void persMemoryRead(uint16_t address_, uint8_t *out_byte );
void persMemoryWrite(uint16_t address_, uint8_t in_byte );
void BnHapticActuator_init();
void BnHapticActuator_turnON(uint8_t strength);
void BnHapticActuator_turnOFF();

#define BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_HMI_LED_SETUP do{ pinMode(STATUS_SENSOR_HMI_LED_P, OUTPUT); pinMode(STATUS_SENSOR_HMI_LED_M, OUTPUT); digitalWrite(STATUS_SENSOR_HMI_LED_M, LOW); }while(0)
#define BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_HMI_LED_ON do{ digitalWrite(STATUS_SENSOR_HMI_LED_P, HIGH); }while(0)
#define BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_HMI_LED_OFF do{ digitalWrite(STATUS_SENSOR_HMI_LED_P, LOW); }while(0)

typedef union
{
    float number;
    unsigned char bytes[4];
} float_converter;

typedef union
{
    uint32_t numberU;
    int32_t numberS;
} int_converter;

#ifdef BLE_COMMUNICATION

#define BLE_MAX_RETRIES 3
#define BLE_SCAN_TIME_MS 30000

#define BLE_MIN_INTERVAL    0x0006 // 7.5ms (7.5 / 1.25)
#define BLE_MAX_INTERVAL    0x0018 // 30ms (30 / 1.25)
#define BLE_SLAVE_LATENCY              0x0000 // No slave latency.
#define BLE_CONN_SUPERVISION_TIMEOUT   0x03E8 // 10s.
#define BLE_PERIPHERAL_APPEARANCE  BLE_APPEARANCE_UNKNOWN

#define BODYNODES_BLE_DEVICE_NAME_TAG "Bodynode"
#define BLE_CHARACTERISTIC_ORIABS_MAX_LEN 16
#define BLE_CHARACTERISTIC_ACCREL_MAX_LEN 12
#define BLE_CHARACTERISTIC_ANGVELREL_MAX_LEN 12
#define BLE_CHARACTERISTIC_GLOVE_MAX_LEN 9
#define BLE_CHARACTERISTIC_SHOE_MAX_LEN 1
#define BLE_CHARACTERISTIC_MAX_LEN 20

#define BN_NODE_SPECIFIC_BN_BLE_NODE_COMMUNICATOR_HMI_SETUP do{ pinMode(STATUS_CONNECTION_HMI_LED_P, OUTPUT); pinMode(STATUS_CONNECTION_HMI_LED_M, OUTPUT); digitalWrite(STATUS_CONNECTION_HMI_LED_P, 0); }while(0)
#define BN_NODE_SPECIFIC_BN_BLE_NODE_COMMUNICATOR_HMI_LED_ON do{ digitalWrite(STATUS_CONNECTION_HMI_LED_P, HIGH); }while(0)
#define BN_NODE_SPECIFIC_BN_BLE_NODE_COMMUNICATOR_HMI_LED_OFF do{ digitalWrite(STATUS_CONNECTION_HMI_LED_P, 0); }while(0)

void BnBLENodeCommunicator_init();
uint8_t BnBLENodeCommunicator_checkAllOk( uint8_t current_conn_status );
void BnBLENodeCommunicator_sendAllMessages(JsonArray &bnc_messages_list);

#endif // BLE_COMMUNICATION

#ifdef WIFI_COMMUNICATION

#define BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_HMI_SETUP do{ pinMode(STATUS_CONNECTION_HMI_LED_P, OUTPUT); }while(0)
#define BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_HMI_LED_ON do{ digitalWrite(STATUS_CONNECTION_HMI_LED_P, LED_DT_ON); }while(0)
#define BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_HMI_LED_OFF do{ digitalWrite(STATUS_CONNECTION_HMI_LED_P, 0); }while(0)

bool tryConnectWifi(String ssid, String password);
void printWifiStatus();
IPAddress getIPAdressFromStr(String ip_address_str);

#define BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_INIT_WIFI WiFi.disconnect();
#define BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_UDP_OBJ UDP
#define BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_BEGIN_MULTICAST \
      wnc_multicast_connector.begin(BODYNODES_MULTICAST_PORT);     \
      wnc_multicast_connector.joinMulticast(multicastIP); // Listen to the Multicast 

#endif // WIFI_COMMUNICATION

#endif //__BN_NODE_SPECIFIC_H
