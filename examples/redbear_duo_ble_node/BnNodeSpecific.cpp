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

#include "BnNodeSpecific.h"

#ifdef WIFI_COMMUNICATION

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

#endif // WIFI_COMMUNICATION

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

#ifdef BLE_COMMUNICATION

#define HEX_TO_UINT8(c) ( ((c) >= '0' && (c) <= '9') ? ((c) - '0') : \
                          ((c) >= 'a' && (c) <= 'f') ? ((c) - 'a' + 10) : \
                          ((c) >= 'A' && (c) <= 'F') ? ((c) - 'A' + 10) : 0 )
//              8     13     18     23
// "00 00 CC A0 -00 00 -10 00 -80 00 -00 80 5F 9B 34 FB"

#define UUID_TO_UINT8(uuid_str) { \
    HEX_PAIR_TO_UINT8(uuid_str[0], uuid_str[1]), HEX_PAIR_TO_UINT8(uuid_str[2], uuid_str[3]), \
    HEX_PAIR_TO_UINT8(uuid_str[4], uuid_str[5]), HEX_PAIR_TO_UINT8(uuid_str[6], uuid_str[7]), \
    HEX_PAIR_TO_UINT8(uuid_str[9], uuid_str[10]), HEX_PAIR_TO_UINT8(uuid_str[11], uuid_str[12]), \
    HEX_PAIR_TO_UINT8(uuid_str[14], uuid_str[15]), HEX_PAIR_TO_UINT8(uuid_str[16], uuid_str[17]), \
    HEX_PAIR_TO_UINT8(uuid_str[19], uuid_str[20]), HEX_PAIR_TO_UINT8(uuid_str[21], uuid_str[22]), \
    HEX_PAIR_TO_UINT8(uuid_str[24], uuid_str[25]), HEX_PAIR_TO_UINT8(uuid_str[26], uuid_str[27]), \
    HEX_PAIR_TO_UINT8(uuid_str[28], uuid_str[29]), HEX_PAIR_TO_UINT8(uuid_str[30], uuid_str[31]), \
    HEX_PAIR_TO_UINT8(uuid_str[32], uuid_str[33]), HEX_PAIR_TO_UINT8(uuid_str[34], uuid_str[35])  \
}

#define HEX_PAIR_TO_UINT8(high, low) ((HEX_TO_UINT8(high) << 4) | HEX_TO_UINT8(low))


static uint8_t BNC_appearance[2] = { 
  LOW_BYTE(BLE_PERIPHERAL_APPEARANCE), 
  HIGH_BYTE(BLE_PERIPHERAL_APPEARANCE) 
};

static uint8_t BNC_change[4] = {
  0x00, 0x00, 0xFF, 0xFF
};

static uint8_t BNC_conn_param[8] = {
  LOW_BYTE(BLE_MIN_INTERVAL), HIGH_BYTE(BLE_MIN_INTERVAL), 
  LOW_BYTE(BLE_MAX_INTERVAL), HIGH_BYTE(BLE_MAX_INTERVAL), 
  LOW_BYTE(BLE_SLAVE_LATENCY), HIGH_BYTE(BLE_SLAVE_LATENCY), 
  LOW_BYTE(BLE_CONN_SUPERVISION_TIMEOUT), HIGH_BYTE(BLE_CONN_SUPERVISION_TIMEOUT)
};

static advParams_t BNC_adv_params = {
  .adv_int_min   = 0x0030,
  .adv_int_max   = 0x0030,
  .adv_type      = BLE_GAP_ADV_TYPE_ADV_IND,
  .dir_addr_type = BLE_GAP_ADDR_TYPE_PUBLIC,
  .dir_addr      = {0,0,0,0,0,0},
  .channel_map   = BLE_GAP_ADV_CHANNEL_MAP_ALL,
  .filter_policy = BLE_GAP_ADV_FP_ANY
};

// ble peripheral advertising data
static uint8_t BNC_adv_data[] = {
  0x02,
  BLE_GAP_AD_TYPE_FLAGS,
  BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE,   
  
  0x11,
  BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE,
  0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x00, 0x00, 0x3d, 0x71 
};

// ble peripheral scan respond data
static uint8_t BNC_scan_response[] = {
  0x09,
  BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME,
  'B', 'o',  'd', 'y', 'n', 'o', 'd', 'e'
};


static uint8_t sBodynodesService_uuid[] = UUID_TO_UINT8( BLE_BODYNODES_SERVICE_UUID );

static uint8_t sPlayerChara_uuid[] = UUID_TO_UINT8( BLE_BODYNODES_CHARA_PLAYER_UUID );
static uint8_t sBodypartChara_uuid[] = UUID_TO_UINT8( BLE_BODYNODES_CHARA_BODYPART_UUID );

static uint8_t sOrientationAbsChara_uuid[] = UUID_TO_UINT8( BLE_BODYNODES_CHARA_ORIENTATION_ABS_VALUE_UUID );
static uint8_t sAccelerationRelChara_uuid[] = UUID_TO_UINT8( BLE_BODYNODES_CHARA_ACCELERATION_REL_VALUE_UUID );
static uint8_t sAngularvelocityRelChara_uuid[] = UUID_TO_UINT8( BLE_BODYNODES_CHARA_ANGULARVELOCITY_REL_VALUE_UUID );
static uint8_t sGloveChara_uuid[] = UUID_TO_UINT8( BLE_BODYNODES_CHARA_GLOVE_VALUE_UUID );
static uint8_t sShoeChara_uuid[] = UUID_TO_UINT8( BLE_BODYNODES_CHARA_SHOE_UUID );

// This boolean just forces the Player and Bodypart to be set once
// This is by specifications. The reason is that BLE communication is quite unique in nature and it is just
// more complex to handle changing parts
static bool sPlayerBodypartSet = false;

static bool sIsConnected = false;

// Characteristic value handle
static uint16_t sPlayerChara_handle = 0x0000;
static uint16_t sBodypartChara_handle = 0x0000;

static uint16_t sOrientationAbsChara_handle = 0x0000;
static uint16_t sAccelerationRelChara_handle = 0x0000;
static uint16_t sAngularvelocityRelChara_handle = 0x0000;
static uint16_t sGloveChara_handle = 0x0000;
static uint16_t sShoeChara_handle = 0x0000;

static uint8_t sPlayerChara_data[BLE_CHARACTERISTIC_MAX_LEN] = { 0x00 };
static uint8_t sBodypartChara_data[BLE_CHARACTERISTIC_MAX_LEN] = { 0x00 };

static uint8_t sPlayerChara_dataLength = 0;
static uint8_t sBodypartChara_dataLength = 0;

// Function to start advertising
void startAdv(void) {
    ble.startAdvertising();
}

/**
 * @brief Connect handle.
 *
 * @param[in]  status   BLE_STATUS_CONNECTION_ERROR or BLE_STATUS_OK.
 * @param[in]  handle   Connect handle.
 *
 * @retval None
 */
void BNC_deviceConnectedCallback(BLEStatus_t status, uint16_t handle) {
    switch (status) {
        case BLE_STATUS_OK:
            sIsConnected = true;
            break;
        default: break;
    }
}

// Callback when disconnected
void BNC_deviceDisconnectedCallback(uint16_t handle) {
    DEBUG_PRINTLN("Disconnected");

    // Restart advertising after disconnection
    startAdv();
    sIsConnected = false;
}

/**
 * @brief Callback for reading event.
 *
 * @note  If characteristic contains client characteristic configuration,then client characteristic configuration handle is value_handle+1.
 *        Now can't add user_descriptor.
 *
 * @param[in]  value_handle    
 * @param[in]  buffer 
 * @param[in]  buffer_size    Ignore it.
 *
 * @retval  Length of current attribute value.
 */
uint16_t BNC_gattReadCallback(uint16_t value_handle, uint8_t * buffer, uint16_t buffer_size) {   
    uint8_t characteristic_len = 0;
    DEBUG_PRINT("Read value handler: ");
    DEBUG_PRINTLN_HEX(value_handle);
    
    if ( value_handle ==  sPlayerChara_handle ){
        DEBUG_PRINTLN("Returning player ");
        memcpy(buffer, sPlayerChara_data, sPlayerChara_dataLength);
        characteristic_len = sPlayerChara_dataLength;
    } else if (value_handle ==  sBodypartChara_handle ){
        DEBUG_PRINTLN("Returning bodypart ");
        memcpy(buffer, sBodypartChara_data, sBodypartChara_dataLength);
        characteristic_len = sBodypartChara_dataLength;
    } 
    
    return characteristic_len;
}

void BnBLENodeCommunicator_init(){

    ble.init();

    // Set ble advertising parameters
    ble.setAdvertisementParams(&BNC_adv_params);

    // Set ble advertising and scan respond data
    ble.setAdvertisementData(sizeof(BNC_adv_data), BNC_adv_data);
    ble.setScanResponseData(sizeof(BNC_scan_response), BNC_scan_response);
    ble.onDataReadCallback(BNC_gattReadCallback);
    ble.onConnectedCallback(BNC_deviceConnectedCallback);
    ble.onDisconnectedCallback(BNC_deviceDisconnectedCallback);

    // Add GAP service and characteristics
    ble.addService(BLE_UUID_GAP);
    ble.addCharacteristic(BLE_UUID_GAP_CHARACTERISTIC_DEVICE_NAME, ATT_PROPERTY_READ, (uint8_t*)BODYNODES_BLE_DEVICE_NAME_TAG, sizeof(BODYNODES_BLE_DEVICE_NAME_TAG));
    ble.addCharacteristic(BLE_UUID_GAP_CHARACTERISTIC_APPEARANCE, ATT_PROPERTY_READ, BNC_appearance, sizeof(BNC_appearance));
    ble.addCharacteristic(BLE_UUID_GAP_CHARACTERISTIC_PPCP, ATT_PROPERTY_READ, BNC_conn_param, sizeof(BNC_conn_param));

    // Add GATT service and characteristics
    ble.addService(BLE_UUID_GATT);
    ble.addCharacteristic(BLE_UUID_GATT_CHARACTERISTIC_SERVICE_CHANGED, ATT_PROPERTY_INDICATE, BNC_change, sizeof(BNC_change));

    ble.addService(sBodynodesService_uuid);

    sPlayerChara_handle = ble.addCharacteristicDynamic(
        sPlayerChara_uuid,
        ATT_PROPERTY_READ,
        (uint8_t*)" ",
        BLE_CHARACTERISTIC_MAX_LEN);

    sBodypartChara_handle = ble.addCharacteristicDynamic(
        sBodypartChara_uuid,
        ATT_PROPERTY_READ,
        (uint8_t*)" ",
        BLE_CHARACTERISTIC_MAX_LEN);

    sOrientationAbsChara_handle = ble.addCharacteristicDynamic(
        sOrientationAbsChara_uuid,
        ATT_PROPERTY_NOTIFY,
        (uint8_t*)" ",
        BLE_CHARACTERISTIC_ORIABS_MAX_LEN);

    sAccelerationRelChara_handle = ble.addCharacteristicDynamic(
        sAccelerationRelChara_uuid,
        ATT_PROPERTY_NOTIFY,
        (uint8_t*)" ",
        BLE_CHARACTERISTIC_ACCREL_MAX_LEN);

    sAngularvelocityRelChara_handle = ble.addCharacteristicDynamic(
        sAngularvelocityRelChara_uuid,
        ATT_PROPERTY_NOTIFY,
        (uint8_t*)" ",
        BLE_CHARACTERISTIC_ANGVELREL_MAX_LEN);

#ifdef GLOVE_SENSOR_ON_BOARD

    sGloveChara_handle = ble.addCharacteristicDynamic(
        sGloveChara_uuid,
        ATT_PROPERTY_NOTIFY,
        (uint8_t*)" ",
        BLE_CHARACTERISTIC_GLOVE_MAX_LEN);

#endif /*GLOVE_SENSOR_ON_BOARD*/

#ifdef SHOE_SENSOR_ON_BOARD

    sShoeChara_handle = ble.addCharacteristicDynamic(
        sShoeChara_uuid,
        ATT_PROPERTY_NOTIFY,
        (uint8_t*)" ",
        BLE_CHARACTERISTIC_SHOE_MAX_LEN);

#endif /*SHOE_SENSOR_ON_BOARD*/

    // Start advertising
    startAdv();
    sPlayerChara_dataLength = 0;
    sBodypartChara_dataLength = 0;
    sPlayerBodypartSet = false;
    DEBUG_PRINTLN("BLE service started and advertising.");
    sIsConnected = false;

}

uint8_t BnBLENodeCommunicator_checkAllOk( uint8_t current_conn_status ){
    if (current_conn_status == CONNECTION_STATUS_NOT_CONNECTED){
        DEBUG_PRINTLN("Not connected");
        delay(1000);
        return CONNECTION_STATUS_WAITING_ACK;
    } else {
        if( sIsConnected ) {
            return CONNECTION_STATUS_CONNECTED;
        } else {
            return CONNECTION_STATUS_NOT_CONNECTED;
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

            sPlayerChara_dataLength = message_json[MESSAGE_PLAYER_TAG].as<String>().length();
            sBodypartChara_dataLength = message_json[MESSAGE_BODYPART_TAG].as<String>().length();
            memcpy( sPlayerChara_data, message_json[MESSAGE_PLAYER_TAG].as<String>().c_str() , sPlayerChara_dataLength+1 );
            memcpy( sBodypartChara_data, message_json[MESSAGE_BODYPART_TAG].as<String>().c_str() , sBodypartChara_dataLength+1 );

            sPlayerBodypartSet = true;
        }
        String sensortype_str = message_json[MESSAGE_SENSORTYPE_TAG].as<String>();
        String value_str = message_json[MESSAGE_VALUE_TAG].as<String>();

        DEBUG_PRINT("message = ");
        String output;
        serializeJson(message_json, output);
        DEBUG_PRINTLN(output);

        if(sensortype_str == SENSORTYPE_ORIENTATION_ABS_TAG) {
            uint8_t bytes_message[BLE_CHARACTERISTIC_ORIABS_MAX_LEN];
            convertStringArrayToFloatBytes(value_str.c_str(), value_str.length(), bytes_message, BLE_CHARACTERISTIC_ORIABS_MAX_LEN/4);
            ble.sendNotify(sOrientationAbsChara_handle, bytes_message, BLE_CHARACTERISTIC_ORIABS_MAX_LEN);
        } else if(sensortype_str == SENSORTYPE_ACCELERATION_REL_TAG) {
            uint8_t bytes_message[BLE_CHARACTERISTIC_ACCREL_MAX_LEN];
            convertStringArrayToFloatBytes(value_str.c_str(), value_str.length(), bytes_message, BLE_CHARACTERISTIC_ACCREL_MAX_LEN/4);
            ble.sendNotify(sAccelerationRelChara_handle, bytes_message, BLE_CHARACTERISTIC_ACCREL_MAX_LEN);
        } else if(sensortype_str == SENSORTYPE_GLOVE_TAG) {
            uint8_t bytes_message[BLE_CHARACTERISTIC_GLOVE_MAX_LEN];
            convertStringArrayToUInt8Bytes(value_str.c_str(), value_str.length(), bytes_message, BLE_CHARACTERISTIC_GLOVE_MAX_LEN);
            ble.sendNotify(sGloveChara_handle, bytes_message, BLE_CHARACTERISTIC_GLOVE_MAX_LEN);
        } else if(sensortype_str == SENSORTYPE_SHOE_TAG) {
            uint8_t bytes_message[BLE_CHARACTERISTIC_SHOE_MAX_LEN];
            convertStringArrayToUInt8Bytes(value_str.c_str(), value_str.length(), bytes_message, BLE_CHARACTERISTIC_SHOE_MAX_LEN);
            ble.sendNotify(sShoeChara_handle, bytes_message, BLE_CHARACTERISTIC_SHOE_MAX_LEN);
        } else if(sensortype_str == SENSORTYPE_ANGULARVELOCITY_REL_TAG) {
            uint8_t bytes_message[BLE_CHARACTERISTIC_ANGVELREL_MAX_LEN];
            convertStringArrayToFloatBytes(value_str.c_str(), value_str.length(), bytes_message, BLE_CHARACTERISTIC_ANGVELREL_MAX_LEN/4);
            ble.sendNotify(sAngularvelocityRelChara_handle, bytes_message, BLE_CHARACTERISTIC_ANGVELREL_MAX_LEN);            
        } 
    }
    uint8_t num_messages = bnc_messages_list.size();
    for (uint8_t index = 0; index<num_messages; ++index) {
        bnc_messages_list.remove(0);
    }

}

#endif
