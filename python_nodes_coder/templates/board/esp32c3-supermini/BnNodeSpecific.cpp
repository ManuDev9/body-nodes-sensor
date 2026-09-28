/**
* MIT License
*
* Copyright (c) 2023-2025 Manuel Bottini
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

#include "BnNodeSpecific.h"

// Implements Specification Version Dev 1.0
// Sensortypes: orientation_abs, acceleration_rel, glove
// Board: ESP32C3 Dev Module

typedef union
{
    float number;
    unsigned char bytes[4];
} float_converter;

typedef union
{
    uint32_t numberU;
    int32_t numberS;
} int_converter;

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

void BnHapticActuator_init() {
    pinMode(HAPTIC_MOTOR_PIN_P, OUTPUT);
}

void BnHapticActuator_turnON(uint8_t strength) {
    (void) strength;
    digitalWrite(HAPTIC_MOTOR_PIN_P, HIGH);
}

void BnHapticActuator_turnOFF() {
    digitalWrite(HAPTIC_MOTOR_PIN_P, LOW);
}

static void convertStringArrayToFloatBytes(char const *message_str, uint8_t slength, uint8_t *bytes_message, uint8_t num_floats) {

    char message_tmp[slength-1];
    memcpy(message_tmp, message_str+1, slength-1); // removing the [ and ] at the beginning and the end
    message_tmp[slength-2] = '\0';
    DEBUG_PRINT("convertStringArrayToFloatBytes message_tmp = ");
    DEBUG_PRINTLN(message_tmp);

    float fl_array[num_floats];
    int idl = 0;
    char* token = strtok(message_tmp, ", ");
    while (token != NULL && idl < num_floats) {
        fl_array[idl] = atof(token);  // Convert the token to float
        token = strtok(NULL, ", "); // Move to the next token
        ++idl;
    }

    // BIG ENDIAN CONVERION
    float_converter fc;
    for( uint8_t idf = 0; idf < num_floats; ++idf ){
        fc.number = fl_array[idf];
        bytes_message[0 + 4*idf] = fc.bytes[3];
        bytes_message[1 + 4*idf] = fc.bytes[2];
        bytes_message[2 + 4*idf] = fc.bytes[1];
        bytes_message[3 + 4*idf] = fc.bytes[0]; 
    }
}


static void convertStringArrayToUInt8Bytes(char const *message_str, uint8_t slength, uint8_t *bytes_message, uint8_t num_uints) {

    char message_tmp[slength-1];
    memcpy(message_tmp, message_str+1, slength-1); // removing the [ and ] at the beginning and the end
    message_tmp[slength-2] = '\0';
    DEBUG_PRINT("convertStringArrayToFloatBytes message_tmp = ");
    DEBUG_PRINTLN(message_tmp);

    int idl = 0;
    char* token = strtok(message_tmp, ", ");
    while (token != NULL && idl < num_uints) {
        bytes_message[idl] = (uint8_t)atoi(token);  // Convert the token to int and then to uint8_t
        token = strtok(NULL, ", "); // Move to the next token
        ++idl;
    }
}


#ifdef WIFI_COMMUNICATION

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
              DEBUG_PRINT(WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "something");
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

#endif // WIFI_COMMUNICATION


#ifdef BLE_COMMUNICATION
 
#include <NimBLEDevice.h>

#define BLE_CHARACTERISTIC_MAX_LEN 20
#define BLE_CHARACTERISTIC_ORIABS_MAX_LEN 16
#define BLE_CHARACTERISTIC_ACCREL_MAX_LEN 12
#define BLE_CHARACTERISTIC_ANGVELREL_MAX_LEN 12
#define BLE_CHARACTERISTIC_GLOVE_MAX_LEN 9
#define BLE_CHARACTERISTIC_SHOE_MAX_LEN 1
#define BLE_CHARACTERISTIC_MAX_LEN 20
#define BODYNODES_BLE_DEVICE_NAME_TAG "Bodynode"

#define BLE_MIN_INTERVAL    0x0006 // 7.5ms (7.5 / 1.25)
#define BLE_MAX_INTERVAL    0x0018 // 30ms (30 / 1.25)
#define BLE_SLAVE_LATENCY              0x0000 // No slave latency.
#define BLE_CONN_SUPERVISION_TIMEOUT   0x03E8 // 10s.

// ---------------------------------------------------------------
// External config expected from your shared config header
// (same names as the original code, now string/plain values):
//
//   BN_BLE_SERVICE_UUID
//   BN_BLE_CHARA_PLAYER_UUID
//   BN_BLE_CHARA_BODYPART_UUID
//   BN_BLE_CHARA_ORIENTATION_ABS_VALUE_UUID
//   BN_BLE_CHARA_ACCELERATION_REL_VALUE_UUID
//   BN_BLE_CHARA_ANGULARVELOCITY_REL_VALUE_UUID
//   BN_BLE_CHARA_GLOVE_VALUE_UUID
//   BN_BLE_CHARA_SHOE_UUID
//
//   BN_CONNECTION_STATUS_NOT_CONNECTED
//   BN_CONNECTION_STATUS_WAITING_ACK
//   BN_CONNECTION_STATUS_CONNECTED
//
//   BN_MESSAGE_PLAYER_TAG / BN_MESSAGE_BODYPART_TAG /
//   BN_MESSAGE_SENSORTYPE_TAG / BN_MESSAGE_VALUE_TAG
//
//   BN_SENSORTYPE_ORIENTATION_ABS_TAG / BN_SENSORTYPE_ACCELERATION_REL_TAG /
//   BN_SENSORTYPE_ANGULARVELOCITY_REL_TAG / BN_SENSORTYPE_GLOVE_TAG /
//   BN_SENSORTYPE_SHOE_TAG
//
// ---------------------------------------------------------------
  
// Same "set once" behaviour as the original: player/bodypart are
// fixed for the lifetime of a BLE session.
static bool sPlayerBodypartSet = false;
static bool sIsConnected = false;
 
static NimBLEServer*         sServer               = nullptr;
static NimBLEAdvertising*    sAdvertising          = nullptr;
 
static NimBLECharacteristic* sPlayerChara          = nullptr;
static NimBLECharacteristic* sBodypartChara        = nullptr;
static NimBLECharacteristic* sOrientationAbsChara  = nullptr;
static NimBLECharacteristic* sAccelerationRelChara = nullptr;
static NimBLECharacteristic* sAngularvelocityRelChara = nullptr;
#ifdef GLOVE_SENSOR_ON_BOARD
static NimBLECharacteristic* sGloveChara           = nullptr;
#endif
#ifdef SHOE_SENSOR_ON_BOARD
static NimBLECharacteristic* sShoeChara            = nullptr;
#endif
 
static uint8_t sPlayerChara_data[BLE_CHARACTERISTIC_MAX_LEN]   = { 0x00 };
static uint8_t sBodypartChara_data[BLE_CHARACTERISTIC_MAX_LEN] = { 0x00 };
static uint8_t sPlayerChara_dataLength   = 0;
static uint8_t sBodypartChara_dataLength = 0;
 
// Function to (re)start advertising
void startAdv(void) {
    NimBLEDevice::startAdvertising();
}
 
/**
 * @brief Server connect/disconnect callbacks.
 *        Replaces BNC_deviceConnectedCallback / BNC_deviceDisconnectedCallback.
 */
class BNC_ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        sIsConnected = true;
        pServer->updateConnParams(connInfo.getConnHandle(),
                            BLE_MIN_INTERVAL, BLE_MAX_INTERVAL,
                            BLE_SLAVE_LATENCY, BLE_CONN_SUPERVISION_TIMEOUT);
    }
 
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        DEBUG_PRINTLN("Disconnected");
        sIsConnected = false;
        // Restart advertising after disconnection
        startAdv();
    }
};
 
static BNC_ServerCallbacks sServerCallbacks;
 
void BnBLENodeCommunicator_init(){
 
    NimBLEDevice::init(BODYNODES_BLE_DEVICE_NAME_TAG);
 
    sServer = NimBLEDevice::createServer();
    sServer->setCallbacks(&sServerCallbacks);
 
    // Custom Bodynodes service
    NimBLEService* bodynodesService = sServer->createService(BN_BLE_SERVICE_UUID);
 
    sPlayerChara = bodynodesService->createCharacteristic(
        BN_BLE_CHARA_PLAYER_UUID,
        NIMBLE_PROPERTY::READ);
    sPlayerChara->setValue((uint8_t*)" ", 1);
 
    sBodypartChara = bodynodesService->createCharacteristic(
        BN_BLE_CHARA_BODYPART_UUID,
        NIMBLE_PROPERTY::READ);
    sBodypartChara->setValue((uint8_t*)" ", 1);
 
    sOrientationAbsChara = bodynodesService->createCharacteristic(
        BN_BLE_CHARA_ORIENTATION_ABS_VALUE_UUID,
        NIMBLE_PROPERTY::NOTIFY);
 
    sAccelerationRelChara = bodynodesService->createCharacteristic(
        BN_BLE_CHARA_ACCELERATION_REL_VALUE_UUID,
        NIMBLE_PROPERTY::NOTIFY);
 
    sAngularvelocityRelChara = bodynodesService->createCharacteristic(
        BN_BLE_CHARA_ANGULARVELOCITY_REL_VALUE_UUID,
        NIMBLE_PROPERTY::NOTIFY);
 
#ifdef GLOVE_SENSOR_ON_BOARD
    sGloveChara = bodynodesService->createCharacteristic(
        BN_BLE_CHARA_GLOVE_VALUE_UUID,
        NIMBLE_PROPERTY::NOTIFY);
#endif /* GLOVE_SENSOR_ON_BOARD */
 
#ifdef SHOE_SENSOR_ON_BOARD
    sShoeChara = bodynodesService->createCharacteristic(
        BN_BLE_CHARA_SHOE_UUID,
        NIMBLE_PROPERTY::NOTIFY);
#endif /* SHOE_SENSOR_ON_BOARD */
 
    bodynodesService->start();
 
    // Advertising: flags + 128-bit service UUID + scan response with name
    sAdvertising = NimBLEDevice::getAdvertising();
    sAdvertising->addServiceUUID(BN_BLE_SERVICE_UUID);
    sAdvertising->setName(BODYNODES_BLE_DEVICE_NAME_TAG);
 
    // Preferred connection parameters (equivalent of BNC_conn_param).
    // Units: 1.25ms per tick for interval, same numeric defines as before.
    sAdvertising->setMinInterval(0x0030);
    sAdvertising->setMaxInterval(0x0030);
    sAdvertising->setPreferredParams(BLE_MIN_INTERVAL, BLE_MAX_INTERVAL);
 


    // Start advertising
    startAdv();
 
    sPlayerChara_dataLength = 0;
    sBodypartChara_dataLength = 0;
    sPlayerBodypartSet = false;
    sIsConnected = false;
 
    DEBUG_PRINTLN("BLE service started and advertising.");
}
 
uint8_t BnBLENodeCommunicator_checkAllOk( uint8_t current_conn_status ){
    if (current_conn_status == BN_CONNECTION_STATUS_NOT_CONNECTED){
        DEBUG_PRINTLN("Not connected");
        delay(1000);
        return BN_CONNECTION_STATUS_WAITING_ACK;
    } else {
        if( sIsConnected ) {
            return BN_CONNECTION_STATUS_CONNECTED;
        } else {
            return BN_CONNECTION_STATUS_NOT_CONNECTED;
        }
    }
}
 
void BnBLENodeCommunicator_sendAllMessages(JsonArray &bnc_messages_list){
    if(bnc_messages_list.size() == 0) {
        //DEBUG_PRINTLN("No messages to send");
        return;
    }
    for (JsonObject message_json : bnc_messages_list) {
        if(!sPlayerBodypartSet) {
 
            String player_str   = message_json[BN_MESSAGE_PLAYER_TAG].as<String>();
            String bodypart_str = message_json[BN_MESSAGE_BODYPART_TAG].as<String>();
 
            sPlayerChara_dataLength   = player_str.length();
            sBodypartChara_dataLength = bodypart_str.length();
 
            memcpy( sPlayerChara_data, player_str.c_str(), sPlayerChara_dataLength + 1 );
            memcpy( sBodypartChara_data, bodypart_str.c_str(), sBodypartChara_dataLength + 1 );
 
            // Update the characteristic's stored value so subsequent
            // reads (GATT READ requests) return the current data.
            sPlayerChara->setValue(sPlayerChara_data, sPlayerChara_dataLength);
            sBodypartChara->setValue(sBodypartChara_data, sBodypartChara_dataLength);
 
            sPlayerBodypartSet = true;
        }
 
        String sensortype_str = message_json[BN_MESSAGE_SENSORTYPE_TAG].as<String>();
        String value_str      = message_json[BN_MESSAGE_VALUE_TAG].as<String>();
 
        DEBUG_PRINT("message = ");
        String output;
        serializeJson(message_json, output);
        DEBUG_PRINTLN(output);
 
        if(sensortype_str == BN_SENSORTYPE_ORIENTATION_ABS_TAG) {
            uint8_t bytes_message[BLE_CHARACTERISTIC_ORIABS_MAX_LEN];
            convertStringArrayToFloatBytes(value_str.c_str(), value_str.length(), bytes_message, BLE_CHARACTERISTIC_ORIABS_MAX_LEN/4);
            sOrientationAbsChara->setValue(bytes_message, BLE_CHARACTERISTIC_ORIABS_MAX_LEN);
            sOrientationAbsChara->notify();
        } else if(sensortype_str == BN_SENSORTYPE_ACCELERATION_REL_TAG) {
            uint8_t bytes_message[BLE_CHARACTERISTIC_ACCREL_MAX_LEN];
            convertStringArrayToFloatBytes(value_str.c_str(), value_str.length(), bytes_message, BLE_CHARACTERISTIC_ACCREL_MAX_LEN/4);
            sAccelerationRelChara->setValue(bytes_message, BLE_CHARACTERISTIC_ACCREL_MAX_LEN);
            sAccelerationRelChara->notify();
#ifdef GLOVE_SENSOR_ON_BOARD
        } else if(sensortype_str == BN_SENSORTYPE_GLOVE_TAG) {
            uint8_t bytes_message[BLE_CHARACTERISTIC_GLOVE_MAX_LEN];
            convertStringArrayToUInt8Bytes(value_str.c_str(), value_str.length(), bytes_message, BLE_CHARACTERISTIC_GLOVE_MAX_LEN);
            sGloveChara->setValue(bytes_message, BLE_CHARACTERISTIC_GLOVE_MAX_LEN);
            sGloveChara->notify();
#endif
#ifdef SHOE_SENSOR_ON_BOARD
        } else if(sensortype_str == BN_SENSORTYPE_SHOE_TAG) {
            uint8_t bytes_message[BLE_CHARACTERISTIC_SHOE_MAX_LEN];
            convertStringArrayToUInt8Bytes(value_str.c_str(), value_str.length(), bytes_message, BLE_CHARACTERISTIC_SHOE_MAX_LEN);
            sShoeChara->setValue(bytes_message, BLE_CHARACTERISTIC_SHOE_MAX_LEN);
            sShoeChara->notify();
#endif
        } else if(sensortype_str == BN_SENSORTYPE_ANGULARVELOCITY_REL_TAG) {
            uint8_t bytes_message[BLE_CHARACTERISTIC_ANGVELREL_MAX_LEN];
            convertStringArrayToFloatBytes(value_str.c_str(), value_str.length(), bytes_message, BLE_CHARACTERISTIC_ANGVELREL_MAX_LEN/4);
            sAngularvelocityRelChara->setValue(bytes_message, BLE_CHARACTERISTIC_ANGVELREL_MAX_LEN);
            sAngularvelocityRelChara->notify();
        }
    }
 
    uint8_t num_messages = bnc_messages_list.size();
    for (uint8_t index = 0; index < num_messages; ++index) {
        bnc_messages_list.remove(0);
    }
}
 
#endif /* BLE_COMMUNICATION */

