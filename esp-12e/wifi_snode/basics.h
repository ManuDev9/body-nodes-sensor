/**
* MIT License
* 
* Copyright (c) 2021 Manuel Bottini
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
#include <ESP8266WiFi.h> 

#ifndef __WIFI_INTERMEDIATE_NODE_BASIC_H
#define __WIFI_INTERMEDIATE_NODE_BASIC_H

/*
 * A Full body suit is 11 nodes
 * We will take this as max numbers a number of nodes that is higher
 */
#define MAX_BODYNODES_NUMBER 15
#define MAX_BODYPART_LENGTH 20
#define MAX_TYPE_LENGTH 20
#define MAX_VALUE_LENGTH 100
#define MAX_BUFF_LENGTH 100


struct Action {
  int type;
  int strength;
  int duration_ms;
  char bodypart[MAX_BODYPART_LENGTH];
  String message;
};

struct Connection {
  IPAddress remote_ip;
  uint16_t remote_port;
};

struct Connections {
  Connection bodypart[MAX_BODYNODES_NUMBER];
  unsigned int num_connections;
};

//ESP-12E
#define BUZZER_FREQ 1000 //Specified in Hz
#define STATUS_SENSOR_HMI_LED_P 2
#define STATUS_CONNECTION_HMI_LED_P 0
#define HAPTIC_MOTOR_PIN_P 14

//Body Part
#define BODY_HEAD_INT             1
#define BODY_BODY_INT             5
#define BODY_UPPERBODY_INT        17
#define BODY_LOWERBODY_INT        18

#define NODE_BODY_PART BODY_LOWERBODY_INT

#define BODY_HEAD_TAG             "head"
#define BODY_BODY_TAG             "body"
#define BODY_UPPERBODY_TAG        "upperbody"
#define BODY_LOWERBODY_TAG        "lowerbody"

//Action
#define ACTION_ACTION_TAG       "action"
#define ACTION_DURATIONMS_TAG   "duration_ms"
#define ACTION_STRENGTH_TAG     "strength"
#define ACTION_BODYPART_TAG     "bodypart"

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

#define SERVER_PORT_WITH_NODE 12345
#define SERVER_PORT_WITH_AP 12344

#define WIFI_NODE_DEVICE_NAME_TAG "WIFI_snode"

#if NODE_BODY_PART == BODY_HEAD_INT
  #define NODE_BODY_PART_TAG BODY_HEAD_TAG
#elif NODE_BODY_PART == BODY_BODY_INT
  #define NODE_BODY_PART_TAG BODY_BODY_TAG
#elif NODE_BODY_PART == BODY_UPPERBODY_INT
  #define NODE_BODY_PART_TAG BODY_UPPERBODY_TAG
#elif NODE_BODY_PART == BODY_LOWERBODY_INT
  #define NODE_BODY_PART_TAG BODY_LOWERBODY_TAG
#endif //NODE_BODY_PART

#define AP_SSID NODE_BODY_PART_TAG
#define AP_PASS "bodynodes1"

#define MESSAGES_SEND_PERIOD_MS 30

#endif //__WIFI_INTERMEDIATE_NODE_BASIC_H
