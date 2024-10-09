/**
* MIT License
*
* Copyright (c) 2024 Manuel Bottini
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

#include "BnBLENodeCommunicator.h"

#ifdef BLE_COMMUNICATION

void BnBLENodeCommunicator::init(){
    bnc_status_LED.on = false;
    bnc_status_LED.lastToggle = millis();
  
    bnc_connection_data.setDisconnected();
    bnc_connection_data.last_sent_time = 0;

    bnc_messages_list = bnc_messages_doc.to<JsonArray>();

    BN_NODE_SPECIFIC_BN_BLE_NODE_COMMUNICATOR_HMI_SETUP;
    BN_NODE_SPECIFIC_BN_BLE_NODE_COMMUNICATOR_HMI_LED_OFF;
    BnBLENodeCommunicator_init();
}

bool BnBLENodeCommunicator::checkAllOk(){
    checkStatus();
    bnc_connection_data.conn_status = BnBLENodeCommunicator_checkAllOk(bnc_connection_data.conn_status);
    if(bnc_connection_data.isWaitingACK()){
        return false;
    } else if(bnc_connection_data.isConnected()){
        return true;
    } else { // isDisconnected()
        return false;
    }
}

void BnBLENodeCommunicator::sendAllMessages(){
    BnBLENodeCommunicator_sendAllMessages(bnc_messages_list);
    bnc_connection_data.last_sent_time = millis();
    bnc_messages_doc.garbageCollect();
}

void BnBLENodeCommunicator::addMessage(JsonObject &message){
  if(bnc_messages_list.size() >= MAX_MESSAGES_LIST_LENGTH){
    DEBUG_PRINTLN("Too many messages in list");
    return;
  }
  bnc_messages_list.add(message);

  if(bnc_messages_list.size() == 0) {
      DEBUG_PRINTLN("No messages to added HERE");
      return;
  }

}

void BnBLENodeCommunicator::getActions(JsonArray &actions){
}

void BnBLENodeCommunicator::checkStatus(){
    if(bnc_connection_data.isDisconnected()){
        BN_NODE_SPECIFIC_BN_BLE_NODE_COMMUNICATOR_HMI_LED_OFF;
        bnc_status_LED.on = false;
        bnc_status_LED.lastToggle = millis();
    } else if(bnc_connection_data.isWaitingACK()) {
        if(millis() - bnc_status_LED.lastToggle > 300){
          bnc_status_LED.lastToggle = millis();
          bnc_status_LED.on = !bnc_status_LED.on;
          if(bnc_status_LED.on){
              BN_NODE_SPECIFIC_BN_BLE_NODE_COMMUNICATOR_HMI_LED_ON;
          } else {
              BN_NODE_SPECIFIC_BN_BLE_NODE_COMMUNICATOR_HMI_LED_OFF;
          }
        }
    } else {
        BN_NODE_SPECIFIC_BN_BLE_NODE_COMMUNICATOR_HMI_LED_ON;
        bnc_status_LED.on = true;
        bnc_status_LED.lastToggle = millis();
    }
}

#endif