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

// Implements Specification Version Dev 1.0
// Sensortypes: orientation_abs, acceleration_rel, glove
// Board:  Arduino Nano 33 BLE 

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

typedef struct {
    uint8_t memory[512];  
} Memory_t;

static Memory_t sBlock;

void persMemoryInit() {
    if (!InternalFS.begin()) {
        DEBUG_PRINTLN("Failed to mount file system");
        return;
    }

    Adafruit_LittleFS_Namespace::File file = InternalFS.open("/prefs.bin", Adafruit_LittleFS_Namespace::FILE_O_READ);
    if (!file) {
        DEBUG_PRINTLN("Failed to open file for reading");
        return;
    }

    file.read((uint8_t *)&sBlock, sizeof(sBlock));
    file.close();
}

void persMemoryCommit() {
}

void persMemoryRead(uint16_t address_, uint8_t *out_byte ) {
    if (address_ < sizeof(sBlock.memory)) {
        *out_byte = sBlock.memory[address_];
    } else {
        DEBUG_PRINTLN("Address out of bounds");
    }
}

void persMemoryWrite(uint16_t address_, uint8_t in_byte ) {
if (address_ < sizeof(sBlock.memory)) {
        sBlock.memory[address_] = in_byte;
        
        Adafruit_LittleFS_Namespace::File file = InternalFS.open("/prefs.bin", Adafruit_LittleFS_Namespace::FILE_O_WRITE);
        if (!file) {
            DEBUG_PRINTLN("Failed to open file for writing");
            return;
        }

        file.write((uint8_t *)&sBlock, sizeof(sBlock));
        file.close();
    } else {
        DEBUG_PRINTLN("Address out of bounds");
    }
}

void BnHapticActuator_init() {
    // Not implemeted
}

void BnHapticActuator_turnON(uint8_t strength) {
    // Not implemeted
}

void BnHapticActuator_turnOFF() {
    // Not implemeted    
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

#include <bluefruit.h>

#define CHARA_MAX_LENGTH 20

static BLEService sBodynodesService(BLE_BODYNODES_SERVICE_UUID);

static BLECharacteristic sPlayerChara(BLE_BODYNODES_CHARA_PLAYER_UUID);
static BLECharacteristic sBodypartChara(BLE_BODYNODES_CHARA_BODYPART_UUID);

static BLECharacteristic sOrientationAbsChara(BLE_BODYNODES_CHARA_ORIENTATION_ABS_VALUE_UUID);
static BLECharacteristic sAccelerationRelChara(BLE_BODYNODES_CHARA_ACCELERATION_REL_VALUE_UUID);
static BLECharacteristic sGloveChara(BLE_BODYNODES_CHARA_GLOVE_VALUE_UUID);
static BLECharacteristic sShoeChara(BLE_BODYNODES_CHARA_SHOE_UUID, BLENotify);

// This boolean just forces the Player and Bodypart to be set once
// This is by specifications. The reason is that BLE communication is quite unique in nature and it is just
// more complex to handle changing parts
static bool sPlayerBodypartSet = false;

static bool sIsConnected = false;

// Function to start advertising
void startAdv(void) {
    // Advertising packet
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();

    // Include the service in the advertising data
    Bluefruit.Advertising.addService(sBodynodesService);

    // Include the device name in the scan response
    Bluefruit.ScanResponse.addName();

    // Start advertising indefinitely
    Bluefruit.Advertising.start(0);
}

// Callback when connected
void connect_callback(uint16_t conn_handle) {
    Serial.println("Connected");
    sIsConnected = true;

}

// Callback when disconnected
void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
    Serial.print("Disconnected, reason: ");
    Serial.println(reason);

    // Restart advertising after disconnection
    startAdv();
    sIsConnected = false;
}

void BnBLENodeCommunicator_init(){

    
    pinMode(STATUS_CONNECTION_HMI_LED_P, OUTPUT);
    pinMode(STATUS_CONNECTION_HMI_LED_M, OUTPUT);
    digitalWrite(STATUS_CONNECTION_HMI_LED_P, LOW);
    digitalWrite(STATUS_CONNECTION_HMI_LED_M, LOW);
    
    Bluefruit.begin();
    
    Bluefruit.setName(BLE_BODYNODES_NAME);
    Bluefruit.setTxPower(4); // Max power
    Bluefruit.Periph.setConnectCallback(connect_callback);
    Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  

    // Start the service
    sBodynodesService.begin();
    
    // Add characteristics to the service
    sPlayerChara.setProperties( CHR_PROPS_READ ); // Read properties 
    sPlayerChara.setPermission( SECMODE_OPEN, SECMODE_NO_ACCESS );
    sPlayerChara.setFixedLen( CHARA_MAX_LENGTH );  
    sPlayerChara.begin( );

    sBodypartChara.setProperties( CHR_PROPS_READ ); // Read properties
    sBodypartChara.setPermission( SECMODE_OPEN, SECMODE_NO_ACCESS );
    sBodypartChara.setFixedLen( CHARA_MAX_LENGTH );  
    sBodypartChara.begin( );

    sOrientationAbsChara.setProperties( CHR_PROPS_NOTIFY ); // Notify properties  
    sOrientationAbsChara.setPermission( SECMODE_OPEN, SECMODE_NO_ACCESS ); 
    sOrientationAbsChara.setFixedLen( CHARA_MAX_LENGTH );  
    sOrientationAbsChara.begin( );

    sAccelerationRelChara.setProperties( CHR_PROPS_NOTIFY ); // Notify properties  
    sAccelerationRelChara.setPermission( SECMODE_OPEN, SECMODE_NO_ACCESS ); 
    sAccelerationRelChara.setFixedLen( CHARA_MAX_LENGTH );  
    sAccelerationRelChara.begin( );

#ifdef GLOVE_SENSOR_ON_BOARD
    sGloveChara.setProperties( CHR_PROPS_NOTIFY ); // Notify properties  
    sGloveChara.setPermission( SECMODE_OPEN, SECMODE_NO_ACCESS ); 
    sGloveChara.setFixedLen( CHARA_MAX_LENGTH );  
    sGloveChara.begin( );
#endif /*GLOVE_SENSOR_ON_BOARD*/

#ifdef SHOE_SENSOR_ON_BOARD
    sShoeChara.setProperties( CHR_PROPS_NOTIFY ); // Notify properties  
    sShoeChara.setPermission( SECMODE_OPEN, SECMODE_NO_ACCESS ); 
    sShoeChara.setFixedLen( CHARA_MAX_LENGTH );  
    sShoeChara.begin( );
#endif /*SHOE_SENSOR_ON_BOARD*/
    
    
    // Start advertising
    startAdv();
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
            sPlayerChara.write(message_json[MESSAGE_PLAYER_TAG].as<String>().c_str());
            sBodypartChara.write(message_json[MESSAGE_BODYPART_TAG].as<String>().c_str());
            sPlayerBodypartSet = true;
        }
        String sensortype_str = message_json[MESSAGE_SENSORTYPE_TAG].as<String>();
        String value_str = message_json[MESSAGE_VALUE_TAG].as<String>();

        DEBUG_PRINT("message = ");
        String output;
        serializeJson(message_json, output);
        DEBUG_PRINTLN(output);
        
        if(sensortype_str == SENSORTYPE_ORIENTATION_ABS_TAG) {
            uint8_t bytes_message[16];
            convertStringArrayToFloatBytes(value_str.c_str(), value_str.length(), bytes_message, 4);
            sOrientationAbsChara.write(bytes_message, 16);
            sOrientationAbsChara.notify(&bytes_message, 16);
        } else if(sensortype_str == SENSORTYPE_ACCELERATION_REL_TAG) {
            uint8_t bytes_message[12];
            convertStringArrayToFloatBytes(value_str.c_str(), value_str.length(), bytes_message, 3);
            sAccelerationRelChara.write(bytes_message, 12);
            sAccelerationRelChara.notify(&bytes_message, 12);
        } else if(sensortype_str == SENSORTYPE_GLOVE_TAG) {
            uint8_t bytes_message[9];
            convertStringArrayToUInt8Bytes(value_str.c_str(), value_str.length(), bytes_message, 9);
            sGloveChara.write(bytes_message, 9);
            sGloveChara.notify(&bytes_message, 9);
        } else if(sensortype_str == SENSORTYPE_SHOE_TAG) {
            //TODO sShoeChara.write(message_json[MESSAGE_VALUE_TAG].as<String>().c_str());
            //TODO sShoeChara.notify(&bytes_message, 1);
        } 
    }
    uint8_t num_messages = bnc_messages_list.size();
    for (uint8_t index = 0; index<num_messages; ++index) {
        bnc_messages_list.remove(0);
    }

}

#endif