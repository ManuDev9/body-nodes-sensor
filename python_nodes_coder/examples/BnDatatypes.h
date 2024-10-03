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

#include "BnConstants.h"
#include "BnNodeSpecific.h"

#ifndef __BN_DATATYPES_H
#define __BN_DATATYPES_H

#define MAX_RECEIVED_BYTES_LENGTH 150

struct BnStatusLED {
  bool on;
  unsigned long lastToggle;
};

using BnKey = String;
using BnType = String;
using BnAction = JsonObject;

#ifdef BLE_COMMUNICATION

class BnBLEConnectionData {
public:
    void setDisconnected();
    void setWaitingACK();
    void setConnected();
    bool isWaitingACK();
    bool isDisconnected();
    bool isConnected();
    void cleanBytes();
    
    uint8_t conn_status = CONNECTION_STATUS_NOT_CONNECTED;
    
    unsigned long last_sent_time = 0;
};
#endif

#ifdef WIFI_COMMUNICATION

class BnIPConnectionData {
public:
    void setDisconnected();
    void setWaitingACK();
    void setConnected();
    bool isWaitingACK();
    bool isDisconnected();
    bool isConnected();
    void cleanBytes();
    
    uint8_t conn_status = CONNECTION_STATUS_NOT_CONNECTED;
    IPAddress ip_address;
    bool has_ip_address;
    
    byte received_bytes[MAX_RECEIVED_BYTES_LENGTH];
    uint16_t num_received_bytes = 0;
    
    unsigned long last_sent_time = 0;
    unsigned long last_rec_time = 0;
};

#endif

class BnSensorData {
public:

    void setValues(float values[], BnType sensortype);
    void setValues(int values[], BnType sensortype);
    void getValues(float values[]);
    void getValues(int values[]);
    BnType getType();
    bool isEmpty();

private:
    BnType sd_sensortype = SENSORTYPE_NONE_TAG;
    float sd_values_float[5];
    int16_t sd_values_int[5];
    uint8_t sd_num_values;
};


#endif //__BN_DATATYPES_H