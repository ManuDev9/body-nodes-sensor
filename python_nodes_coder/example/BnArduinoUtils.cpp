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

#include "BnArduinoUtils.h"

void BnPersMemory::init(){
  persMemoryInit();

  uint8_t checkkey[5];
  persMemoryRead(0, &checkkey[0]);
  persMemoryRead(1, &checkkey[1]);
  persMemoryRead(2, &checkkey[2]);
  persMemoryRead(3, &checkkey[3]);
  persMemoryRead(4, &checkkey[4]);

  if(!(checkkey[0] == pm_checkkey[0] &&
    checkkey[1] == pm_checkkey[1] &&
    checkkey[2] == pm_checkkey[2] &&
    checkkey[3] == pm_checkkey[3] &&
    checkkey[4] == pm_checkkey[4])){

    DEBUG_PRINTLN("New memory!");

    // Clear all the data
    clean();

    persMemoryWrite(0, pm_checkkey[0]);
    persMemoryWrite(1, pm_checkkey[1]);
    persMemoryWrite(2, pm_checkkey[2]);
    persMemoryWrite(3, pm_checkkey[3]);
    persMemoryWrite(4, pm_checkkey[4]);
    persMemoryCommit();

    // Sets default values
    setValue(MEMORY_PLAYER_TAG, BODYNODE_PLAYER_TAG_DEFAULT );
    setValue(MEMORY_BODYPART_TAG, BODYNODE_BODYPART_TAG_DEFAULT );
#if defined(GLOVE_SENSOR_ON_SERIAL) || defined(GLOVE_SENSOR_ON_BOARD)
    setValue(MEMORY_BODYPART_GLOVE_TAG, BODYNODE_BODYPART_GLOVE_TAG);
#endif /*defined(GLOVE_SENSOR_ON_SERIAL) || defined(GLOVE_SENSOR_ON_BOARD)*/
#ifdef SHOE_SENSOR_ON_BOARD
    setValue(MEMORY_BODYPART_SHOE_TAG, BODYNODE_BODYPART_SHOE_TAG);
#endif /*SHOE_SENSOR_ON_BOARD*/
    setValue(MEMORY_WIFI_SSID_TAG, BODYNODES_WIFI_SSID_DEFAULT);
    setValue(MEMORY_WIFI_PASSWORD_TAG, BODYNODES_WIFI_PASS_DEFAULT);
    setValue(MEMORY_WIFI_MULTICASTMESSAGE_TAG, BODYNODES_MULTICASTMESSAGE_DEFAULT);

  } else {
    DEBUG_PRINTLN("Already setup memory!");
  }

}

void BnPersMemory::clean(){
  for(uint16_t index=0; index<512; ++index){
    persMemoryWrite(index, 0);
  }
  persMemoryCommit();
}

void BnPersMemory::setValue(String key, String value){
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
  persMemoryWrite(addr_nbytes, len);
  for(uint8_t index = 0; index < len; ++index){
    persMemoryWrite(addr_chars+index, tmp_buf[index]);
  }
  persMemoryCommit();
}

String BnPersMemory::getValue(String key){
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
  persMemoryRead(addr_nbytes, &len);
  char tmp_buf[len];
  for(uint8_t index = 0; index < len; ++index){
    uint8_t tmp;
    persMemoryRead(addr_chars+index, &tmp);
    tmp_buf[index] = static_cast<char>(tmp);
  }

  return tmp_buf;
}
