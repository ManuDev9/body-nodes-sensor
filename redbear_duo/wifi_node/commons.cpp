/**
* MIT License
* 
* Copyright (c) 2021-2023 Manuel Bottini
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

#include "commons.h"

void IPConnectionData::setDisconnected(){ conn_status = CONNECTION_STATUS_NOT_CONNECTED; }
void IPConnectionData::setWaitingACK(){ conn_status = CONNECTION_STATUS_WAITING_ACK; }
void IPConnectionData::setConnected(){ conn_status = CONNECTION_STATUS_CONNECTED; }
bool IPConnectionData::isWaitingACK(){ return conn_status == CONNECTION_STATUS_WAITING_ACK; }
bool IPConnectionData::isDisconnected(){ return conn_status == CONNECTION_STATUS_NOT_CONNECTED; }
bool IPConnectionData::isConnected(){ return conn_status == CONNECTION_STATUS_CONNECTED; }
void IPConnectionData::cleanBytes(){ num_received_bytes = 0; }

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


void PersMemory::init(){
  //EEPROM.begin(512);
  EEPROM.begin();

  char checkkey[5];
  EEPROM.get(0, checkkey);
  if(!(checkkey[0] == pm_checkkey_0 &&
    checkkey[1] == pm_checkkey_1 &&
    checkkey[2] == pm_checkkey_2 &&
    checkkey[3] == pm_checkkey_3 &&
    checkkey[4] == pm_checkkey_4)){

    DEBUG_PRINTLN("New memory!");

    // Clear all the data
    clean();

    EEPROM.write(0, pm_checkkey_0);
    EEPROM.write(1, pm_checkkey_1);
    EEPROM.write(2, pm_checkkey_2);
    EEPROM.write(3, pm_checkkey_3);
    EEPROM.write(4, pm_checkkey_4);
    //EEPROM.commit();
    // Sets default values
    setValue(MEMORY_PLAYER_TAG, BODYNODE_PLAYER_TAG_DEFAULT );
    setValue(MEMORY_BODYPART_TAG, BODYNODE_BODYPART_TAG_DEFAULT );
#ifdef BODYNODE_GLOVE_SENSOR
    setValue(MEMORY_BODYPART_GLOVE_TAG, BODYNODE_BODYPART_GLOVE_TAG);
#endif /*BODYNODE_GLOVE_SENSOR*/
#ifdef BODYNODE_SHOE_SENSOR
    setValue(MEMORY_BODYPART_SHOE_TAG, BODYNODE_BODYPART_SHOE_TAG);
#endif /*BODYNODE_SHOE_SENSOR*/
    setValue(MEMORY_WIFI_SSID_TAG, BODYNODES_WIFI_SSID_DEFAULT);
    setValue(MEMORY_WIFI_PASSWORD_TAG, BODYNODES_WIFI_PASS_DEFAULT);
    setValue(MEMORY_WIFI_MULTICASTMESSAGE_TAG, BODYNODES_MULTICASTMESSAGE_DEFAULT);

  } else {
    DEBUG_PRINTLN("Already setup memory!");
  }

}

void PersMemory::clean(){
  for(uint16_t index=0; index<512; ++index){
    EEPROM.write(index, 0);
  }
  //EEPROM.commit();
}

void PersMemory::setValue(String key, String value){
  uint16_t addr_nbytes = 0;
  uint16_t addr_chars = 0;
  if(key == MEMORY_PLAYER_TAG) {
    addr_nbytes = pm_player_addr_nbytes;
    addr_chars = pm_player_addr_chars;
  } else if(key == MEMORY_BODYPART_TAG) {
    addr_nbytes = pm_bodypart_addr_nbytes;
    addr_chars = pm_bodypart_addr_chars;
  } else if(key == MEMORY_BODYPART_GLOVE_TAG) {
    addr_nbytes = pm_bodypart_glove_addr_nbytes;
    addr_chars = pm_bodypart_glove_addr_chars;
  } else if(key == MEMORY_WIFI_SSID_TAG) {
    addr_nbytes = pm_ssid_addr_nbytes;
    addr_chars = pm_ssid_addr_chars;
  } else if(key == MEMORY_WIFI_PASSWORD_TAG) {
    addr_nbytes = pm_password_addr_nbytes;
    addr_chars = pm_password_addr_chars;
  } else if(key == MEMORY_WIFI_MULTICASTMESSAGE_TAG) {
    addr_nbytes = pm_multicast_message_addr_nbytes;
    addr_chars = pm_multicast_message_addr_chars;
  } else {
    DEBUG_PRINT("Cannot find in memory key = ");
    DEBUG_PRINTLN(key);
    return;
  }
  uint8_t len = value.length()+1;
  char tmp_buf[len];
  value.toCharArray(tmp_buf, len);
  EEPROM.write(addr_nbytes, len);
  for(uint8_t index = 0; index < len; ++index){
    EEPROM.write(addr_chars+index, tmp_buf[index]);
  }
  //EEPROM.commit();
}

String PersMemory::getValue(String key){
  uint16_t addr_nbytes = 0;
  uint16_t addr_chars = 0;
  if(key == MEMORY_PLAYER_TAG) {
    addr_nbytes = pm_player_addr_nbytes;
    addr_chars = pm_player_addr_chars;
  } else if(key == MEMORY_BODYPART_TAG) {
    addr_nbytes = pm_bodypart_addr_nbytes;
    addr_chars = pm_bodypart_addr_chars;
  } else if(key == MEMORY_BODYPART_GLOVE_TAG) {
    addr_nbytes = pm_bodypart_glove_addr_nbytes;
    addr_chars = pm_bodypart_glove_addr_chars;
  } else if(key == MEMORY_WIFI_SSID_TAG) {
    addr_nbytes = pm_ssid_addr_nbytes;
    addr_chars = pm_ssid_addr_chars;
  } else if(key == MEMORY_WIFI_PASSWORD_TAG) {
    addr_nbytes = pm_password_addr_nbytes;
    addr_chars = pm_password_addr_chars;
  } else if(key == MEMORY_WIFI_MULTICASTMESSAGE_TAG) {
    addr_nbytes = pm_multicast_message_addr_nbytes;
    addr_chars = pm_multicast_message_addr_chars;
  } else {
    DEBUG_PRINT("Cannot find in memory key = ");
    DEBUG_PRINTLN(key);
    return "";
  }
  uint8_t len = 0;
  EEPROM.get(addr_nbytes, len);
  char tmp_buf[len];
  for(uint8_t index = 0; index < len; ++index){
    EEPROM.get(addr_chars+index, tmp_buf[index]);
  }

  return tmp_buf;
}
