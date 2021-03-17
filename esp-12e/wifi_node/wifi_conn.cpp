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

#include "wifi_conn.h"

extern "C" {
    #include "user_interface.h"  // Required for wifi_station_connect() to work
}
#define FPM_SLEEP_MAX_TIME 0xFFFFFFF

int mWifiStatus;

char ssid[] = WIFI_SSID;
char password[] = WIFI_PASS;
uint16_t port = SERVER_PORT;     // port number of the server

struct StatusConnLED {
  bool on;
  unsigned long lastToggle;
};

StatusConnLED mStatusConnLED;
IPAddress mServer;
WiFiUDP mUdpConnection;

void initStatusConnectionHMI(){
  pinMode(STATUS_CONNECTION_HMI_LED_P, OUTPUT);
  analogWrite(STATUS_CONNECTION_HMI_LED_P, 0);
  mStatusConnLED.on = false;
  mStatusConnLED.lastToggle = millis();
}

void setStatusConnectionHMI_ON(){
  analogWrite(STATUS_CONNECTION_HMI_LED_P, LED_DT_ON);
  mStatusConnLED.on = true;
  mStatusConnLED.lastToggle = millis();
}

void setStatusConnectionHMI_OFF(){
  analogWrite(STATUS_CONNECTION_HMI_LED_P, 0);
  mStatusConnLED.on = false;
  mStatusConnLED.lastToggle = millis();
}

void setStatusConnectionHMI_BLINK(){
  if(millis() - mStatusConnLED.lastToggle > 300){
    mStatusConnLED.lastToggle = millis();
    mStatusConnLED.on = !mStatusConnLED.on;
    if(mStatusConnLED.on){
      analogWrite(STATUS_CONNECTION_HMI_LED_P, LED_DT_ON);
    } else {
      analogWrite(STATUS_CONNECTION_HMI_LED_P, 0);
    }
  }
}

void setWifiNotConnected(){
  mWifiStatus=WIFI_NOT_CONNECTED;
  setStatusConnectionHMI_OFF();
}

void setWifiConnected(){
  mWifiStatus=WIFI_SERVER_CONNECTED_NO;
}

void setServerNotConnected(){
  mWifiStatus=WIFI_SERVER_CONNECTED_NO;
  setStatusConnectionHMI_OFF();
}

void setServerConnected(){
  mWifiStatus=WIFI_SERVER_CONNECTED_OK;
  setStatusConnectionHMI_ON();
}

void setWaitingACK(){
  mWifiStatus=WIFI_SERVER_WAITING_ACK;
}
  
bool isWifiConnected(){
  if(mWifiStatus==WIFI_NOT_CONNECTED){
    return false;
  } else {
    return true;
  }
}

bool isServerConnected(){
  if(mWifiStatus==WIFI_SERVER_CONNECTED_OK){
    return true;
  } else {
    return false;
  }
}

bool isWaitingACK(){
  if(mWifiStatus==WIFI_SERVER_WAITING_ACK){
    return true;
  } else {
    return false;
  }
}



void checkWifiAndServer(){
  if (WiFi.status() != WL_CONNECTED){
    DEBUG_PRINTLN("Wifi is not ready");  
    setWifiNotConnected();
    tryConnectWifi();
  } else {
    if(isWaitingACK()){
      setStatusConnectionHMI_BLINK();
      if(checkForACK()){
        setServerConnected();
        DEBUG_PRINTLN("Connected to Server");      
      } else {
        sendACK();
      }
    }else if(!isServerConnected()){
      sendACK();
      setWaitingACK();
    }
  }
}

//This in the future will become a way for the node to identify himself with the library
void sendACK(){
  //DEBUG_PRINTLN("Trying to contact Server");
  byte buf_udp [4] = {'A','C','K', '\0'};
  mUdpConnection.beginPacket(mServer, port);
  mUdpConnection.write(buf_udp, 4);
  mUdpConnection.endPacket();
}

bool checkForACK(){
  int size_ = mUdpConnection.parsePacket();
  if(size_ >= 3){
    char buf_udp [4] = {'0','0','0', '\0'};
    mUdpConnection.read((byte*)buf_udp, 3);
    String buf_udp_str = buf_udp;
    DEBUG_PRINTLN("Received from Server = "+buf_udp_str);
    if(strstr (buf_udp_str.c_str(),"ACK")!=NULL){
      //DEBUG_PRINTLN("No ACK from Server");
      return true;
    } else {
      DEBUG_PRINTLN("The message was not an ACK");
    }
  } else {
    //DEBUG_PRINTLN("No ACK received from Server");
  }
  return false;
}

Action checkActionWifi(){
  int size_ = mUdpConnection.parsePacket();
  Action action;
  action.type = ACTION_NOPE_INT;
  action.duration_ms = 0;
  action.strength = 0;
  if(size_ > 0) {
    char buf_udp [MAX_BUFF_LENGTH];
    mUdpConnection.read((byte*)buf_udp, MAX_BUFF_LENGTH);
    String buf_udp_str = buf_udp;
    int indexOpen = buf_udp_str.indexOf("{");
    int indexClose = buf_udp_str.indexOf("}");
    if(indexOpen < 0){
      //No starting point
      return action;
    }
    if(indexOpen > indexClose){
      //we got a starting point after the closing point
      indexClose = buf_udp_str.indexOf("}", indexOpen);
    }
    if(indexClose<0){
      //JSON has not been closed, let's keep what we have and not read from it
      return action;
    }
    buf_udp_str = buf_udp_str.substring(indexOpen, indexClose+1);
    DEBUG_PRINTLN("Received from Server = "+buf_udp_str);
    DynamicJsonDocument actionJson(JSON_OBJECT_SIZE(3) +MAX_BUFF_LENGTH);

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(actionJson, buf_udp_str);
    // Test if parsing succeeds.
    if (error) {
      DEBUG_PRINTLN("No possible to parse the json, error =");
      DEBUG_PRINTLN(error.c_str());
      return action;
    }
    deserializeJson(actionJson, buf_udp_str);
    DEBUG_PRINTLN("actionJson =");
    serializeJson(actionJson, Serial);
    DEBUG_PRINTLN("");
    if(actionJson.containsKey(ACTION_ACTION_TAG)){
      const char* actionName = actionJson[ACTION_ACTION_TAG];
      if(strstr(actionName,ACTION_HAPTIC_TAG)!=NULL){
        if(actionJson.containsKey(ACTION_HAPTIC_DURATIONMS_TAG) &&
            actionJson.containsKey(ACTION_HAPTIC_STRENGTH_TAG))
          {
            action.type = ACTION_HAPTIC_INT;
            action.duration_ms = actionJson[ACTION_HAPTIC_DURATIONMS_TAG];
            action.strength = actionJson[ACTION_HAPTIC_STRENGTH_TAG];
            action.message = buf_udp_str;
        }
      }
    }
  }
  return action;
}

void initWifi(){
  WiFi.disconnect(true);
  WiFi.softAPdisconnect(false);
  WiFi.enableAP(false);
  if(tryConnectWifi()){
    DEBUG_PRINTLN("Connected to Wifi");
    printWifiStatus();
    setWifiConnected();
  }
}

void WiFiOff(){
  DEBUG_PRINTLN("diconnecting client and wifi");
  wifi_station_disconnect();
  wifi_set_opmode(NULL_MODE);
  wifi_set_sleep_type(MODEM_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_do_sleep(FPM_SLEEP_MAX_TIME);
}


void WiFiOn(){
  wifi_fpm_do_wakeup();
  wifi_fpm_close();

  DEBUG_PRINTLN("Reconnecting");
  wifi_set_opmode(STATION_MODE);
  wifi_station_connect();
}

bool tryConnectWifi(){
  // attempt to connect to Wifi network:
  DEBUG_PRINT("Attempting to connect to Network named: ");
  // print the network name (SSID);
  DEBUG_PRINTLN(ssid); 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  //WiFiOff();
  //WiFiOn();
  //WiFi.begin("", "");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED && WiFi.status() != WL_CONNECT_FAILED) {
    // print dots while we wait to connect
    DEBUG_PRINT(".");
    delay(500);
  }
  bool conn = WiFi.status() == WL_CONNECTED;

  if(conn){
    DEBUG_PRINTLN("Waiting for an IP address");
    IPAddress localIP = WiFi.localIP();
    while (localIP[0] == 0) {
      localIP = WiFi.localIP();
      DEBUG_PRINTLN("Waiting for an IP address");
      delay(1000);
    }
    DEBUG_PRINTLN("IP Address obtained");
    mServer = WiFi.gatewayIP();
    mUdpConnection.begin(port);
    return true;
  } else {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(1000);
    int found = WiFi.scanNetworks();
    for (int i=0; i<found; i++) {
        DEBUG_PRINT("SSID: ");
        DEBUG_PRINT(WiFi.SSID(i));
        DEBUG_PRINT(" | Security: ");
        DEBUG_PRINT(WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "something");
        DEBUG_PRINT(" | Channel: ");
        DEBUG_PRINT(WiFi.channel(i));
        DEBUG_PRINT(" | RSSI: ");
        DEBUG_PRINTLN(WiFi.RSSI(i));
    }
    return false;
  }  
}

void insertInOrientationStringQuat(char buf[], int startPos, int prec,double quatVal){
  if(quatVal>=0){
    buf[startPos] = '+';
  } else {
    buf[startPos] = '-';
    quatVal=-quatVal;
  }
  
  for(int i=0; i<=prec+1;i++){
    if(i==1){
      buf[startPos+1+i]= '.';
    } else {
      int val = quatVal;
      buf[startPos+1+i]= val+'0';
      quatVal = quatVal-val;
      quatVal*=10;
    }
  }
  buf[startPos+3+prec]= '|';
}

void sendOrientationValueQuat(imu::Quaternion quat){
  double valW = quat.w();
  double valX = quat.x();
  double valY = quat.y();
  double valZ = quat.z();

  int prec = 3;
  int chars_per_val = prec+4;
  /*
  char buf [chars_per_val*4]={
    '+','0','.','0','0','0','0','|',
    '+','0','.','0','0','0','0','|',
    '+','0','.','0','0','0','0','|',
    '+','0','.','0','0','0','0','\0'};
    */
  char buf [chars_per_val*4];
  DEBUG_PRINT("w = ");
  DEBUG_PRINT(valW);
  DEBUG_PRINT(" | x = ");
  DEBUG_PRINT(valX);
  DEBUG_PRINT(" | y = ");
  DEBUG_PRINTLN(valY);
  DEBUG_PRINT(" | z = ");
  DEBUG_PRINTLN(valZ);

  insertInOrientationStringQuat(buf,0,prec,valW);
  insertInOrientationStringQuat(buf,chars_per_val,prec,valX);
  insertInOrientationStringQuat(buf,chars_per_val*2,prec,valY);
  insertInOrientationStringQuat(buf,chars_per_val*3,prec,valZ);

  buf[chars_per_val*4-1] = '\0';
  DEBUG_PRINT("buf = ");
  DEBUG_PRINTLN(buf);

  //characteristicOrientation.setValue(buf);
  DEBUG_PRINT("w|x|y|z: ");
  DEBUG_PRINTLN(buf);

  String bodyNodeString ="[{\"bodypart\":\"";
  bodyNodeString +=NODE_BODY_PART_TAG;
  bodyNodeString +="\",\"type\":\"orientation\",\"value\":\"";
  bodyNodeString += buf;
  bodyNodeString += "\"}]";
  DEBUG_PRINTLN(bodyNodeString);

  int buf_size = bodyNodeString.length()+1;
  byte buf_udp [buf_size];

  bodyNodeString.getBytes(buf_udp, buf_size);
  mUdpConnection.beginPacket(mServer, port);
  mUdpConnection.write(buf_udp, buf_size);
  mUdpConnection.endPacket(); 
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
