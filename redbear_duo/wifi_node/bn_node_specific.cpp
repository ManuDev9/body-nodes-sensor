/**
* MIT License
*
* Copyright (c) 2023 Manuel Bottini
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

IPAddress getIPAdressFromStr(String ip_address_str) {
  uint8_t len = ip_address_str.length()+1;
  char tmp_buf[len];
  char * tmp_p;
  ip_address_str.toCharArray(tmp_buf, len);

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

void persMemoryInit() {
  // nothing
}

void persMemoryCommit() {
  // nothing
}

void persMemoryRead(uint16_t address_, uint8_t *out_byte ) {
  char tmp;
  EEPROM.get(address_, tmp);
  *out_byte = static_cast<uint8_t>(tmp);
}

void persMemoryWrite(uint16_t address_, uint8_t in_byte ) {
  EEPROM.write(address_, in_byte);
}
