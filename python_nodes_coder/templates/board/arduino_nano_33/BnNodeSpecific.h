/**
* MIT License
*
* Copyright (c) 2021-2024 Manuel Bottini
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
// Sensortypes: orientation_abs, acceleration_rel, glove
// Board: Arduino Nano 33 BLE

#ifndef __BN_NODE_SPECIFIC_H
#define __BN_NODE_SPECIFIC_H

#define BODYNODE_BODYPART_HEX_DEFAULT BODYPART_KATANA_HEX
#define BODYNODE_PLAYER_TAG_DEFAULT  "1"

// COMMUNICATION //

// SENSORS //

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

#define OUT_AXIS_W_ORIE 0
#define OUT_AXIS_X_ORIE 1
#define OUT_AXIS_Y_ORIE 2
#define OUT_AXIS_Z_ORIE 3

#define MUL_AXIS_W_ORIE 1
#define MUL_AXIS_X_ORIE 1
#define MUL_AXIS_Y_ORIE 1
#define MUL_AXIS_Z_ORIE 1

#define OUT_AXIS_X_ACC 0
#define OUT_AXIS_Y_ACC 1
#define OUT_AXIS_Z_ACC 2

#define MUL_AXIS_X_ACC 1
#define MUL_AXIS_Y_ACC 1
#define MUL_AXIS_Z_ACC 1

#define OUT_AXIS_X_ANGVEL 0
#define OUT_AXIS_Y_ANGVEL 1
#define OUT_AXIS_Z_ANGVEL 2

#define MUL_AXIS_X_ANGVEL 1
#define MUL_AXIS_Y_ANGVEL 1
#define MUL_AXIS_Z_ANGVEL 1

// PINS
#define BUZZER_FREQ 1000 //Specified in Hz
#define LED_DT_ON 30 // Duty cicle of LED ON
#define STATUS_SENSOR_HMI_LED_P 2
#define STATUS_CONNECTION_HMI_LED_P 0
#define STATUS_CONNECTION_HMI_LED_M 0  // this is a trick that just works
#define HAPTIC_MOTOR_PIN_P 14

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
  #define BODYNODE_BODYPART_TAG_DEFAULT BODYPART_HEAD_TAG
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

#define BN_NODE_SPECIFIC_BN_GLOVE_SENSOR_MIGNOLO_SENSE_PIN      A4
#define BN_NODE_SPECIFIC_BN_GLOVE_SENSOR_ANULARE_SENSE_PIN      A3
#define BN_NODE_SPECIFIC_BN_GLOVE_SENSOR_MEDIO_SENSE_PIN        A2
#define BN_NODE_SPECIFIC_BN_GLOVE_SENSOR_INDICE_SENSE_PIN       A1
#define BN_NODE_SPECIFIC_BN_GLOVE_SENSOR_POLLICE_SENSE_PIN      A0

#define BN_NODE_SPECIFIC_BN_GLOVE_SENSOR_MIGNOLO_DIGI_PIN       D2
#define BN_NODE_SPECIFIC_BN_GLOVE_SENSOR_ANULARE_DIGI_PIN       D3
#define BN_NODE_SPECIFIC_BN_GLOVE_SENSOR_MEDIO_DIGI_PIN         D4
#define BN_NODE_SPECIFIC_BN_GLOVE_SENSOR_INDICE_DIGI_PIN        D5

// Other node specific utility functions that are defined in the same way
void persMemoryInit();
void persMemoryCommit();
void persMemoryRead(uint16_t address_, uint8_t *out_byte );
void persMemoryWrite(uint16_t address_, uint8_t in_byte );
void BnHapticActuator_init();
void BnHapticActuator_turnON(uint8_t strength);
void BnHapticActuator_turnOFF();

#define BN_NODE_SPECIFIC_BN_ISENSOR_HMI_LED_SETUP 
#define BN_NODE_SPECIFIC_BN_ISENSOR_HMI_LED_ON 
#define BN_NODE_SPECIFIC_BN_ISENSOR_HMI_LED_OFF 

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

#define BN_NODE_SPECIFIC_BN_BLE_NODE_COMMUNICATOR_HMI_SETUP 
#define BN_NODE_SPECIFIC_BN_BLE_NODE_COMMUNICATOR_HMI_LED_ON 
#define BN_NODE_SPECIFIC_BN_BLE_NODE_COMMUNICATOR_HMI_LED_OFF 

void BnBLENodeCommunicator_init();
uint8_t BnBLENodeCommunicator_checkAllOk( uint8_t current_conn_status );
void BnBLENodeCommunicator_sendAllMessages(JsonArray &bnc_messages_list);

#endif // BLE_COMMUNICATION

#endif //__BN_NODE_SPECIFIC_H
