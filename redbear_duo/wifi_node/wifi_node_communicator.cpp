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

#include "wifi_node_communicator.h"

// UTILITY FUNCTIONS
void printWifiStatus();
bool tryConnectWifi(String ssid, String password);
IPAddress getIPAdressFromStr(String ip_address);

void WifiNodeCommunicator::init(){
  WiFi.disconnect();

  pinMode(STATUS_CONNECTION_HMI_LED_P, OUTPUT);
  pinMode(STATUS_CONNECTION_HMI_LED_M, OUTPUT);
  digitalWrite(STATUS_CONNECTION_HMI_LED_P, LOW);
  digitalWrite(STATUS_CONNECTION_HMI_LED_M, LOW);
  wnc_status_LED.on = false;
  wnc_status_LED.lastToggle = millis();
  
  wnc_connection_data.setDisconnected();
  wnc_connection_data.last_sent_time = 0;
  wnc_connection_data.last_rec_time = 0;
  wnc_messages_list = wnc_messages_doc.to<JsonArray>();
  wnc_actions_list = wnc_actions_doc.to<JsonArray>();
}

void WifiNodeCommunicator::setConnectionParams(JsonObject &params){
  PersMemory::setValue(ACTION_SETWIFI_SSID_TAG, params[ACTION_SETWIFI_SSID_TAG].as<String>());
  PersMemory::setValue(ACTION_SETWIFI_PASSWORD_TAG, params[ACTION_SETWIFI_PASSWORD_TAG].as<String>());
}

void WifiNodeCommunicator::receiveBytes(){
  int size_ = wnc_connector.parsePacket();
  if(size_>0){
    wnc_connection_data.num_received_bytes = wnc_connector.read(wnc_connection_data.received_bytes, MAX_RECEIVED_BYTES_LENGTH);
  }
}

bool WifiNodeCommunicator::checkAllOk(){
  bool allok = false;
  checkStatus();
  if(!WiFi.ready()){
    wnc_connection_data.setDisconnected();
  }
  if (wnc_connection_data.isDisconnected()){
    String ssid = PersMemory::getValue(ACTION_SETWIFI_SSID_TAG);
    String password = PersMemory::getValue(ACTION_SETWIFI_PASSWORD_TAG);
    if (!tryConnectWifi(ssid, password)){
      DEBUG_PRINTLN("Not connected to the Wifi");
      wnc_connection_data.setDisconnected();
      delay(1000);
      return false;
    } else {
      DEBUG_PRINTLN("Connected to the Wifi");
      //wnc_connection_data.ip_address = WiFi.gatewayIP();      
      wnc_connector.begin(BODYNODES_PORT);
      IPAddress multicastIP = getIPAdressFromStr(BODYNODES_MULTICASTIP_DEFAULT);
      wnc_connector.begin(BODYNODES_MULTICAST_PORT);
      wnc_connector.joinMulticast(multicastIP); // Listen to the Multicast 
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
    if(checkForACKH()){
      IPAddress server_ipa = wnc_connector.remoteIP();
      wnc_connection_data.ip_address = server_ipa;
      sendACKN();
      wnc_connection_data.setConnected();
      DEBUG_PRINTLN("Connected to Host via Wifi");
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

void WifiNodeCommunicator::addMessage(JsonObject &message){
  if(wnc_messages_list.size() >= MAX_MESSAGES_LIST_LENGTH){
    DEBUG_PRINTLN("Too many messages in list");
    return;
  }
  wnc_messages_list.add(message);
}

void WifiNodeCommunicator::sendAllMessages(){
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

void WifiNodeCommunicator::getActions(JsonArray &actions){
  for (JsonObject action : wnc_actions_list) {
    actions.add(action);
  }
  uint8_t num_actions = wnc_actions_list.size();
  for (uint8_t index = 0; index<num_actions; ++index) {
    wnc_actions_list.remove(0);
  }
  wnc_actions_doc.garbageCollect();
}

void WifiNodeCommunicator::checkForActions(){
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

void WifiNodeCommunicator::checkStatus(){
  if(wnc_connection_data.isDisconnected()){
    digitalWrite(STATUS_CONNECTION_HMI_LED_P, LOW);
    wnc_status_LED.on = false;
    wnc_status_LED.lastToggle = millis();
  } else if(wnc_connection_data.isWaitingACK()) {
    if(millis() - wnc_status_LED.lastToggle > 300){
      wnc_status_LED.lastToggle = millis();
      wnc_status_LED.on = !wnc_status_LED.on;
      if(wnc_status_LED.on){
        digitalWrite(STATUS_CONNECTION_HMI_LED_P, HIGH);
      } else {
        digitalWrite(STATUS_CONNECTION_HMI_LED_P, LOW);
      }
    }
  } else {
    digitalWrite(STATUS_CONNECTION_HMI_LED_P, HIGH);
    wnc_status_LED.on = true;
    wnc_status_LED.lastToggle = millis();
  }
}

//This in the future will become a way for the node to identify himself with the library
void WifiNodeCommunicator::sendACKN(){
  if(millis() - wnc_connection_data.last_sent_time < CONNECTION_ACK_INTERVAL_MS){
    return;
  }  
  byte buf_udp [5] = {'A','C','K','N', '\0'};
  wnc_connector.beginPacket(wnc_connection_data.ip_address, BODYNODES_PORT);
  wnc_connector.write(buf_udp, 5);
  wnc_connector.endPacket();
  wnc_connection_data.last_sent_time = millis();
}

bool WifiNodeCommunicator::checkForACKH(){
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

bool tryConnectWifi(String ssid, String password){
  // attempt to connect to Wifi network:
  DEBUG_PRINT("Attempting to connect to Network named: ");
  // print the network name (SSID);
  DEBUG_PRINTLN(ssid);

  WiFi.off();
  WiFi.on();
  WiFi.clearCredentials();

  bool my_wifi = false;
  WiFiAccessPoint aps[20];
  int found = WiFi.scan(aps, 20);
  for (int i=0; i<found; i++) {
      WiFiAccessPoint& ap = aps[i];
      DEBUG_PRINT("SSID: ");
      DEBUG_PRINT(ap.ssid);
      DEBUG_PRINT(" | Security: ");
      DEBUG_PRINT(ap.security);
      DEBUG_PRINT(" | Channel: ");
      DEBUG_PRINT(ap.channel);
      DEBUG_PRINT(" | RSSI: ");
      DEBUG_PRINTLN(ap.rssi);
      String ap_ssid = ap.ssid; 
      if(ap_ssid.indexOf(ssid)!=-1) {
        my_wifi = true;
      }
  }
  if(!my_wifi) {
    DEBUG_PRINTLN("Couldn't find my wifi");
    return false;
  }
  DEBUG_PRINTLN("Wifi found!"); 
  
  WiFi.setCredentials(ssid, password);
  WiFi.connect();

  while (WiFi.connecting()) {
    // print dots while we wait to connect
    DEBUG_PRINT(".");
    delay(500);
  }
  bool conn = WiFi.ready();

  if(conn){
    DEBUG_PRINTLN("Waiting for an IP address");
    IPAddress localIP = WiFi.localIP();
    while (localIP[0] == 0) {
      localIP = WiFi.localIP();
      DEBUG_PRINTLN("Waiting for an IP address");
      delay(1000);
    }
    DEBUG_PRINTLN("IP Address obtained");
    return true;
  }
  return false;
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  DEBUG_PRINT("Network Name: ");
  DEBUG_PRINTLN(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  DEBUG_PRINT("IP Address: ");
  DEBUG_PRINTLN(ip);
  
  DEBUG_PRINT("Gateway IP address for network ");
  DEBUG_PRINTLN(WiFi.gatewayIP());

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  DEBUG_PRINT("signal strength (RSSI):");
  DEBUG_PRINT(rssi);
  DEBUG_PRINTLN(" dBm");
}


IPAddress getIPAdressFromStr(String ip_address) {
  uint8_t len = ip_address.length()+1;
  char tmp_buf[len];
  char * tmp_p;
  ip_address.toCharArray(tmp_buf, len);

  tmp_p = strtok ( tmp_buf, "." );
  uint8_t ip_address_0 = atoi(tmp_p);
  tmp_p = strtok ( NULL, "." );
  uint8_t ip_address_1 = atoi(tmp_p);
  tmp_p = strtok ( NULL, "." );
  uint8_t ip_address_2 = atoi(tmp_p);
  tmp_p = strtok ( NULL, "." );
  uint8_t ip_address_3 = atoi(tmp_p);
  return IPAddress(ip_address_0, ip_address_1, ip_address_2, ip_address_3);
}
