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

#include "bn_datatypes.h"

void BnIPConnectionData::setDisconnected(){ conn_status = CONNECTION_STATUS_NOT_CONNECTED; }
void BnIPConnectionData::setWaitingACK(){ conn_status = CONNECTION_STATUS_WAITING_ACK; }
void BnIPConnectionData::setConnected(){ conn_status = CONNECTION_STATUS_CONNECTED; }
bool BnIPConnectionData::isWaitingACK(){ return conn_status == CONNECTION_STATUS_WAITING_ACK; }
bool BnIPConnectionData::isDisconnected(){ return conn_status == CONNECTION_STATUS_NOT_CONNECTED; }
bool BnIPConnectionData::isConnected(){ return conn_status == CONNECTION_STATUS_CONNECTED; }
void BnIPConnectionData::cleanBytes(){ num_received_bytes = 0; }

BnType BnSensorData::getType(){
  return sd_sensortype;
}

void BnSensorData::setValues(float values[], BnType sensortype){
  sd_sensortype = sensortype;
  if(sd_sensortype.equals(SENSOR_DATA_TYPE_ORIENTATION_ABS_TAG)) {
    sd_num_values = 4;
  } else if(sd_sensortype.equals(SENSOR_DATA_TYPE_ACCELERATION_REL_TAG)) {
    sd_num_values = 3;
  }
  for(uint8_t index = 0; index<sd_num_values; ++index){
    sd_values_float[index] = values[index];
  }
}

void BnSensorData::setValues(int values[], BnType sensortype){
  sd_sensortype = sensortype;
  if(sd_sensortype.equals(SENSOR_DATA_TYPE_ORIENTATION_ABS_TAG)) {
    sd_num_values = 4;
  } else if(sd_sensortype.equals(SENSOR_DATA_TYPE_ACCELERATION_REL_TAG)) {
    sd_num_values = 3;
  }
  for(uint8_t index = 0; index<sd_num_values; ++index){
    sd_values_int[index] = values[index];
  }
}

void BnSensorData::getValues(float values[]){
  for(uint8_t index = 0; index<sd_num_values; ++index){
    values[index] = sd_values_float[index];
  }
}

void BnSensorData::getValues(int values[]){
  for(uint8_t index = 0; index<sd_num_values; ++index){
    values[index] = sd_values_int[index];
  }
}

bool BnSensorData::isEmpty(){
    return sd_sensortype == SENSOR_DATA_TYPE_NONE_TAG;
}
