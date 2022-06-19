/**
* MIT License
* 
* Copyright (c) 2021-2022 Manuel Bottini
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

#include "constants.h"
#include <Arduino.h> 
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#ifndef __BODYNODES_COMMONS_H
#define __BODYNODES_COMMONS_H

#define MAX_RECEIVED_BYTES_LENGTH 150
#define MAX_MAP_ELEMENTS 25
#define MAX_MAP_STRING_LENGTH 40

#define CONNECTION_STATUS_NOT_CONNECTED  0
#define CONNECTION_STATUS_WAITING_ACK    1
#define CONNECTION_STATUS_CONNECTED      2

struct StatusLED {
  bool on;
  unsigned long lastToggle;
};

class IPConnectionData {
public:
  void setDisconnected();
  void setWaitingACK();
  void setConnected();
  bool isWaitingACK();
  bool isDisconnected();
  void cleanBytes();
  
  uint8_t conn_status = CONNECTION_STATUS_NOT_CONNECTED;
  IPAddress ip_address;
  
  byte received_bytes[MAX_RECEIVED_BYTES_LENGTH];
  uint16_t num_received_bytes = 0;

  unsigned long last_sent_time = 0;
  unsigned long last_rec_time = 0;
};

class PersMemory {
public:
  static void init();
  static void clean();
  static void setValue(String key, String value);
  static String getValue(String key);
private:
  PersMemory(){};

  static constexpr char pm_checkkey_0 = 0x00;
  static constexpr char pm_checkkey_1 = 0x00;
  static constexpr char pm_checkkey_2 = 0x00;
  static constexpr char pm_checkkey_3 = 0x00;
  static constexpr char pm_checkkey_4 = 0x01;

  static constexpr uint16_t pm_player_addr_nbytes = 50;
  static constexpr uint16_t pm_player_addr_chars = 51;
  
  static constexpr uint16_t pm_bodypart_addr_nbytes = 100;
  static constexpr uint16_t pm_bodypart_addr_chars = 101;

  static constexpr uint16_t pm_bodypart_glove_addr_nbytes = 150;
  static constexpr uint16_t pm_bodypart_glove_addr_chars = 151;

  static constexpr uint16_t pm_ssid_addr_nbytes = 200;
  static constexpr uint16_t pm_ssid_addr_chars = 201;

  static constexpr uint16_t pm_password_addr_nbytes = 250;
  static constexpr uint16_t pm_password_addr_chars = 251;

  static constexpr uint16_t pm_server_ip_addr_nbytes = 300;
  static constexpr uint16_t pm_server_ip_addr_chars = 301;

};

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

#endif //__BODYNODES_COMMONS_H
