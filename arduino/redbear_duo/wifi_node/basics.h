/**
* MIT License
* 
* Copyright (c) 2019 Manuel Bottini
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

#include <Arduino.h> 
#include <ArduinoJson.h>

#ifndef __WIFI_NODE_BASIC_H
#define __WIFI_NODE_BASIC_H

#define MAX_BUFF_LENGTH 100

struct Action {
  int type;
  int strength;
  int duration_ms;
};

//RedBear DUO
#define STATUS_SENSOR_HMI_LED_P 6
#define STATUS_SENSOR_HMI_LED_M 5
#define STATUS_CONNECTION_HMI_LED_P 3
#define STATUS_CONNECTION_HMI_LED_M 2
#define HAPTIC_MOTOR_PIN_P 10
#define HAPTIC_MOTOR_PIN_M 11

//Body Part
#define BODY_HEAD_INT             1
#define BODY_HAND_LEFT_INT        2
#define BODY_FOREARM_LEFT_INT     3
#define BODY_UPPERARM_LEFT_INT    4
#define BODY_BODY_INT             5
#define BODY_FOREARM_RIGHT_INT    6
#define BODY_UPPERARM_RIGHT_INT   7
#define BODY_HAND_RIGHT_INT       8
#define BODY_LOWERLEG_LEFT_INT    9
#define BODY_UPPERLEG_LEFT_INT    10
#define BODY_FOOT_LEFT_INT        11
#define BODY_LOWERLEG_RIGHT_INT   12
#define BODY_UPPERLEG_RIGHT_INT   13
#define BODY_FOOT_RIGHT_INT       14
#define BODY_UNTAGGED_INT         15
#define BODY_KATANA_INT           16

#define NODE_BODY_PART BODY_UPPERARM_LEFT_INT

#define BODY_HEAD_TAG             "head"
#define BODY_HAND_LEFT_TAG        "hand_left"
#define BODY_FOREARM_LEFT_TAG     "forearm_left"
#define BODY_UPPERARM_LEFT_TAG    "upperarm_left"
#define BODY_BODY_TAG             "body"
#define BODY_FOREARM_RIGHT_TAG    "forearm_right"
#define BODY_UPPERARM_RIGHT_TAG   "upperarm_right"
#define BODY_HAND_RIGHT_TAG       "hand_right"
#define BODY_LOWERLEG_LEFT_TAG    "lowerleg_left"
#define BODY_UPPERLEG_LEFT_TAG    "upperleg_left"
#define BODY_FOOT_LEFT_TAG        "shoe_left"
#define BODY_LOWERLEG_RIGHT_TAG   "lowerleg_right"
#define BODY_UPPERLEG_RIGHT_TAG   "upperleg_right"
#define BODY_FOOT_RIGHT_TAG       "shoe_right"
#define BODY_UNTAGGED_TAG         "untagged"
#define BODY_KATANA_TAG           "katana"

//Action
#define ACTION_ACTION_TAG            "action"
#define ACTION_HAPTIC_DURATIONMS_TAG "duration_ms"
#define ACTION_HAPTIC_STRENGTH_TAG   "strength"

#define ACTION_NOPE_INT              0
#define ACTION_HAPTIC_INT            1
#define ACTION_HAPTIC_TAG            "haptic"

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

#define WIFI_SSID "BodyNodesHotspot"
#define WIFI_PASS "bodynodes1"

#define SERVER_PORT 12345 

#define WIFI_NODE_DEVICE_NAME_TAG "WIFI_node"


#if NODE_BODY_PART == BODY_HEAD_INT
  #define NODE_BODY_PART_TAG BODY_HEAD_TAG
#elif NODE_BODY_PART == BODY_HAND_LEFT_INT
  #define NODE_BODY_PART_TAG BODY_HAND_LEFT_TAG
#elif NODE_BODY_PART == BODY_FOREARM_LEFT_INT
  #define NODE_BODY_PART_TAG BODY_FOREARM_LEFT_TAG
#elif NODE_BODY_PART == BODY_UPPERARM_LEFT_INT
  #define NODE_BODY_PART_TAG BODY_UPPERARM_LEFT_TAG
#elif NODE_BODY_PART == BODY_BODY_INT
  #define NODE_BODY_PART_TAG BODY_BODY_TAG
#elif NODE_BODY_PART == BODY_FOREARM_RIGHT_INT
  #define NODE_BODY_PART_TAG BODY_FOREARM_RIGHT_TAG
#elif NODE_BODY_PART == BODY_UPPERARM_RIGHT_INT
  #define NODE_BODY_PART_TAG BODY_UPPERARM_RIGHT_TAG
#elif NODE_BODY_PART == BODY_HAND_RIGHT_INT
  #define NODE_BODY_PART_TAG BODY_HAND_RIGHT_TAG
#elif NODE_BODY_PART == BODY_LOWERLEG_LEFT_INT
  #define NODE_BODY_PART_TAG BODY_LOWERLEG_LEFT_TAG
#elif NODE_BODY_PART == BODY_UPPERLEG_LEFT_INT
  #define NODE_BODY_PART_TAG BODY_UPPERLEG_LEFT_TAG
#elif NODE_BODY_PART == BODY_FOOT_LEFT_INT
  #define NODE_BODY_PART_TAG BODY_FOOT_LEFT_TAG
#elif NODE_BODY_PART == BODY_LOWERLEG_RIGHT_INT
  #define NODE_BODY_PART_TAG BODY_LOWERLEG_RIGHT_TAG
#elif NODE_BODY_PART == BODY_UPPERLEG_RIGHT_INT
  #define NODE_BODY_PART_TAG BODY_UPPERLEG_RIGHT_TAG
#elif NODE_BODY_PART == BODY_FOOT_RIGHT_INT
  #define NODE_BODY_PART_TAG BODY_FOOT_RIGHT_TAG
#elif NODE_BODY_PART == BODY_UNTAGGED_INT
  #define NODE_BODY_PART_TAG BODY_UNTAGGED_INT
#elif NODE_BODY_PART == BODY_KATANA_INT
  #define NODE_BODY_PART_TAG BODY_KATANA_TAG  
#endif //NODE_BODY_PART

#endif //__WIFI_NODE_BASIC_H
