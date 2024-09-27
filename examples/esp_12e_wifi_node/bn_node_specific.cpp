/**
* MIT License
*
* Copyright (c) 2023-2024 Manuel Bottini
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

#include "bn_node_specific.h"

// Implements Specification Version Dev 1.0
// Sensortypes: orientation_abs, acceleration_rel, glove
// Board: ESP-12E

bool tryConnectWifi(String ssid, String password){
  if(WiFi.status() == WL_CONNECTED) {
    return true;
  }
  
  // attempt to connect to Wifi network:
  DEBUG_PRINT("Attempting to connect to Network named: ");
  // print the network name (SSID);
  DEBUG_PRINTLN(ssid);
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

IPAddress getIPAdressFromStr(String ip_address_str) {
  IPAddress ipAddress;
  ipAddress.fromString(ip_address_str);
  return ipAddress;
}

void persMemoryInit() {
  EEPROM.begin(512);
}

void persMemoryCommit() {
  EEPROM.commit();
}

void persMemoryRead(uint16_t address_, uint8_t *out_byte ) {
  char tmp;
  EEPROM.get(address_, tmp);
  *out_byte = static_cast<uint8_t>(tmp);
}

void persMemoryWrite(uint16_t address_, uint8_t in_byte ) {
  EEPROM.write(address_, in_byte);
}
