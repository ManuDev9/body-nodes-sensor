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

#ifndef __BLE_NODE_BASIC_H
#define __BLE_NODE_BASIC_H

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

#define NODE_DEVICE_NAME_TAG "BLE_Node"
#define NODE_BODYPART_FULL_TAG BODY_FOREARM_RIGHT_TAG
#define NODE_ACTION_FULL_TAG "hap:" BODY_FOREARM_RIGHT_TAG
#define NODE_BODYPART_FULL_LEN 13
#define NODE_ACTION_FULL_LEN NODE_BODYPART_FULL_LEN+4


//Action
#define ACTION_NOPE_INT              0
#define ACTION_HAPTIC_INT            1

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


#define SENSOR_READ_INTERVAL_MS 60

#endif //__BLE_NODE_BASIC_H
