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

#include "basics.h"
#include "sensor.h"
#include "wifi_conn.h"
#include "actuators.h"
#include "messages_mgr.h"

unsigned long lastSendMessage;

void initStatusHMI(){
  initStatusSensorHMI();
  initStatusConnectionHMI();
}

void setup() {  
  //Initialize serial and wait for port to open:
  Serial.begin(921600);
  setWifiNotConnected();

  initActuators();
  initStatusHMI();
  initSensor();
  initWifi_BothMode();
  initMessages();
  lastSendMessage = 0;
}

void loop() {
  if(!checkSensorInit()){
    return;
  }  
  if(!isCalibrationOK()){
  }
  checkWifiAndServer();
  if(isServerConnected()){
    if(checkReadFromSensor()){
      const char* mtype = "orientation";
      const char* mbodypart = NODE_BODY_PART_TAG;
      const int index_bp = get_index_bodypart(mbodypart);
      store_message_quat(index_bp, mtype, getReadQuat());
    }
    // Action action = checkActionWifi(); // TODO fix the action for the other nodes
    // setAction(action);
  }
  makeActions();

  JsonArray jsonArray;
  get_ap_udp_packets(jsonArray);
  for(JsonVariant elem : jsonArray) {
    JsonObject message = elem.as<JsonObject>();
    const char* mtype = message["type"];
    const char* mvalue = message["value"];
    const char* mbodypart = message["bodypart"];
    const int index_bp = get_index_bodypart(mbodypart);
    store_message(index_bp, mtype, mvalue);
  }

  // Messages will be sent every 30 milliseconds
  if(millis() - lastSendMessage > MESSAGES_SEND_PERIOD_MS){
    lastSendMessage = millis();
    sendToWifi(getAllMessages());
  }  
}
