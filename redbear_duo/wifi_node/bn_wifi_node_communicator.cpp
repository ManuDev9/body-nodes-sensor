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

#include "bn_wifi_node_communicator.h"

void BnWifiNodeCommunicator::init(){
  BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_INIT_WIFI

  pinMode(STATUS_CONNECTION_HMI_LED_P, OUTPUT);
  pinMode(STATUS_CONNECTION_HMI_LED_M, OUTPUT);
  BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_WRITE_STATUS_PIN_FUNCTION(STATUS_CONNECTION_HMI_LED_P, LOW);
  BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_WRITE_STATUS_PIN_FUNCTION(STATUS_CONNECTION_HMI_LED_M, LOW);
  wnc_status_LED.on = false;
  wnc_status_LED.lastToggle = millis();
  
  wnc_connection_data.setDisconnected();
  wnc_connection_data.last_sent_time = 0;
  wnc_connection_data.last_rec_time = 0;
  wnc_connection_data.has_ip_address = false;

  wnc_multicast_data.setDisconnected();
  wnc_multicast_data.last_sent_time = 0;
  wnc_multicast_data.last_rec_time = 0;

  wnc_messages_list = wnc_messages_doc.to<JsonArray>();
  wnc_actions_list = wnc_actions_doc.to<JsonArray>();
}

void BnWifiNodeCommunicator::setConnectionParams(JsonObject &params){
  BnPersMemory::setValue(MEMORY_WIFI_SSID_TAG, params[ACTION_SETWIFI_SSID_TAG].as<String>());
  BnPersMemory::setValue(MEMORY_WIFI_PASSWORD_TAG, params[ACTION_SETWIFI_PASSWORD_TAG].as<String>());
  BnPersMemory::setValue(MEMORY_WIFI_MULTICASTMESSAGE_TAG, params[ACTION_SETWIFI_MULTICASTMESSAGE_TAG].as<String>());
}

void BnWifiNodeCommunicator::receiveBytes(){
  int size_ = wnc_connector.parsePacket();
  if(size_>0){
    wnc_connection_data.num_received_bytes = wnc_connector.read(wnc_connection_data.received_bytes, MAX_RECEIVED_BYTES_LENGTH);
  }

  int size_m = wnc_multicast_connector.parsePacket();
  //DEBUG_PRINTLN(WiFi.gatewayIP());
  if(size_m>0){
    //DEBUG_PRINT("I received some packets of size =  ");
    //DEBUG_PRINTLN(size_m);
    wnc_multicast_data.num_received_bytes = wnc_multicast_connector.read(wnc_multicast_data.received_bytes, MAX_RECEIVED_BYTES_LENGTH);
  }

}

bool BnWifiNodeCommunicator::checkAllOk(){
  bool allok = false;
  checkStatus();
  if(!WiFi.ready()){
    wnc_connection_data.setDisconnected();
  }
  if (wnc_connection_data.isDisconnected()){
    String ssid = BnPersMemory::getValue(MEMORY_WIFI_SSID_TAG);
    String password = BnPersMemory::getValue(MEMORY_WIFI_PASSWORD_TAG);
    if (!tryConnectWifi(ssid, password)){
      DEBUG_PRINTLN("Not connected to the Wifi");
      wnc_connection_data.setDisconnected();
      delay(1000);
      return false;
    } else {
      DEBUG_PRINTLN("Connected to the Wifi");
      //wnc_connection_data.ip_address = WiFi.gatewayIP();      
      wnc_connector.begin(BODYNODES_PORT);
      IPAddress multicastIP = getIPAdressFromStr(BODYNODES_MULTICASTGROUP_DEFAULT);
      BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_BEGIN_MULTICAST
      wnc_multicast_data.setConnected();
      printWifiStatus();
    }
    wnc_connection_data.setWaitingACK();
  }
  
  receiveBytes();
  //unsigned long time_prev = millis();
  //unsigned long time_diff = millis() - time_prev;
  //DEBUG_PRINT("time diff =  ");
  //DEBUG_PRINTLN(time_diff);

  if(wnc_connection_data.isWaitingACK()){
    //DEBUG_PRINTLN("isWaitingACK");
    if(checkForMulticastBN()){
      saveHostInfo();
    }
    if(hasHostInfo()) {
      sendACKN();
      if(checkForACKH()){
        wnc_connection_data.setConnected();
        DEBUG_PRINTLN("Connected to Host via Wifi");
      }
    }
  } else {
    // Connected to wifi and server
    if(millis() - wnc_connection_data.last_sent_time > CONNECTION_KEEP_ALIVE_SEND_INTERVAL_MS){
      sendACKN();
    }
    if(millis() - wnc_connection_data.last_rec_time > CONNECTION_KEEP_ALIVE_REC_INTERVAL_MS){
      wnc_connection_data.setDisconnected();
    }
    if(!checkForACKH()) {
      checkForActions();
    }
    wnc_connection_data.cleanBytes();
    allok = true;
  }
  return allok;
}

void BnWifiNodeCommunicator::addMessage(JsonObject &message){
  if(wnc_messages_list.size() >= MAX_MESSAGES_LIST_LENGTH){
    DEBUG_PRINTLN("Too many messages in list");
    return;
  }
  wnc_messages_list.add(message);
}

void BnWifiNodeCommunicator::sendAllMessages(){
  if(wnc_messages_list.size() == 0) {
    return;
  }
  uint16_t tot_bytes = wnc_messages_doc.memoryUsage()+1;
  uint8_t buf_udp[tot_bytes];

  uint16_t real_tot_bytes = serializeJson(wnc_messages_doc, buf_udp, tot_bytes);

  //DEBUG_PRINT("sendAllMessages wnc_messages_list tot_bytes = ");
  //DEBUG_PRINT(tot_bytes);
  //DEBUG_PRINT(" , real_tot_bytes = ");
  //DEBUG_PRINTLN(real_tot_bytes);

  wnc_connector.beginPacket(wnc_connection_data.ip_address, BODYNODES_PORT);
  wnc_connector.write(buf_udp, real_tot_bytes);
  wnc_connector.endPacket();
  wnc_connection_data.last_sent_time = millis();
  //wnc_messages_doc.clear();
  uint8_t num_messages = wnc_messages_doc.size();
  for (uint8_t index = 0; index<num_messages; ++index) {
    wnc_messages_list.remove(0);
  }
  wnc_messages_doc.garbageCollect();
}

void BnWifiNodeCommunicator::getActions(JsonArray &actions){
  for (JsonObject action : wnc_actions_list) {
    actions.add(action);
  }
  uint8_t num_actions = wnc_actions_list.size();
  for (uint8_t index = 0; index<num_actions; ++index) {
    wnc_actions_list.remove(0);
  }
  wnc_actions_doc.garbageCollect();
}

void BnWifiNodeCommunicator::checkForActions(){
  //DEBUG_PRINT("wnc_connection_data.num_received_bytes = ");
  //DEBUG_PRINTLN(wnc_connection_data.num_received_bytes);
  if(wnc_connection_data.num_received_bytes > 0) {
    //DEBUG_PRINTLN("Checking for actions");
    // Deserialize the JSON document
    DynamicJsonDocument actionDoc(MAX_ACTION_BYTES);
    DeserializationError error = deserializeJson(actionDoc, wnc_connection_data.received_bytes);
    if (error) {
      DEBUG_PRINTLN("No possible to parse the json, error =");
      DEBUG_PRINTLN(error.c_str());
      return;
    }
    wnc_actions_list.add(actionDoc);    
  }
}

void BnWifiNodeCommunicator::checkStatus(){
  if(wnc_connection_data.isDisconnected()){
    BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_WRITE_STATUS_PIN_FUNCTION(STATUS_CONNECTION_HMI_LED_P, LOW);
    wnc_status_LED.on = false;
    wnc_status_LED.lastToggle = millis();
  } else if(wnc_connection_data.isWaitingACK()) {
    if(millis() - wnc_status_LED.lastToggle > 300){
      wnc_status_LED.lastToggle = millis();
      wnc_status_LED.on = !wnc_status_LED.on;
      if(wnc_status_LED.on){
        BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_WRITE_STATUS_PIN_FUNCTION(STATUS_CONNECTION_HMI_LED_P, HIGH);
      } else {
        BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_WRITE_STATUS_PIN_FUNCTION(STATUS_CONNECTION_HMI_LED_P, LOW);
      }
    }
  } else {
    BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_WRITE_STATUS_PIN_FUNCTION(STATUS_CONNECTION_HMI_LED_P, HIGH);
    wnc_status_LED.on = true;
    wnc_status_LED.lastToggle = millis();
  }
}

//This in the future will become a way for the node to identify himself with the library
void BnWifiNodeCommunicator::sendACKN(){
  if(millis() - wnc_connection_data.last_sent_time < CONNECTION_ACK_INTERVAL_MS){
    return;
  }  
  byte buf_udp [5] = {'A','C','K','N', '\0'};
  wnc_connector.beginPacket(wnc_connection_data.ip_address, BODYNODES_PORT);
  wnc_connector.write(buf_udp, 5);
  wnc_connector.endPacket();
  wnc_connection_data.last_sent_time = millis();
}

bool BnWifiNodeCommunicator::checkForACKH(){
  if(wnc_connection_data.num_received_bytes >= 4){
    for(uint16_t index = 0; index<wnc_connection_data.num_received_bytes-3; ++index){
      if( wnc_connection_data.received_bytes[index] == 'A' && wnc_connection_data.received_bytes[index+1] == 'C'
        && wnc_connection_data.received_bytes[index+2] == 'K' && wnc_connection_data.received_bytes[index+3] == 'H') {
        //DEBUG_PRINTLN("ACKH from Host");
        wnc_connection_data.last_rec_time = millis();
        return true;
      }
    }
    DEBUG_PRINTLN("The message was not an ACKH");
  } else {
    //DEBUG_PRINTLN("No ACK received from Server");
  }
  return false;
}

bool BnWifiNodeCommunicator::checkForMulticastBN() {
  if(wnc_multicast_data.num_received_bytes >= 2){
    for(uint16_t index = 0; index<wnc_multicast_data.num_received_bytes-1; ++index){
      if( wnc_multicast_data.received_bytes[index] == 'B' && wnc_multicast_data.received_bytes[index+1] == 'N') {
        wnc_multicast_data.last_rec_time = millis();
        wnc_multicast_data.cleanBytes();
        return true;
      }
    }
    //DEBUG_PRINTLN("The message was not an BN mutlicast");
  } else {
    //DEBUG_PRINTLN("No BN received from Host");
  }
  wnc_multicast_data.cleanBytes();
  return false;
}

void BnWifiNodeCommunicator::saveHostInfo(){
  wnc_connection_data.has_ip_address = true;
  IPAddress server_ipa = wnc_multicast_connector.remoteIP();
  wnc_connection_data.ip_address = server_ipa;
}

bool BnWifiNodeCommunicator::hasHostInfo(){
  //DEBUG_PRINT("hasHostInfo = ");
  //DEBUG_PRINTLN(wnc_connection_data.has_ip_address);
  return wnc_connection_data.has_ip_address;
}
