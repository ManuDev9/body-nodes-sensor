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

#define _FS_LOGLEVEL_               1
#define NANO33BLE_FS_SIZE_KB        512
#define FORCE_REFORMAT              false

// You need to install the NanoBLEFlashPrefs package from Tools->Manage Libraries...
#include "NanoBLEFlashPrefs.h"

typedef struct {
    uint8_t memory[512];  
} Memory_t;

static NanoBLEFlashPrefs sFlashPrefs;
static Memory_t sBlock;

void persMemoryInit() {
    sFlashPrefs.readPrefs(&sBlock, sizeof(sBlock));
}

void persMemoryCommit() {
}

void persMemoryRead(uint16_t address_, uint8_t *out_byte ) {
    *out_byte = sBlock.memory[address_];
}

void persMemoryWrite(uint16_t address_, uint8_t in_byte ) {
    sBlock.memory[address_] = in_byte;
    sFlashPrefs.writePrefs(&sBlock, sizeof(sBlock));
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

#include <ArduinoBLE.h>

#define CHARA_MAX_LENGTH 20

static BLEService sBodynodesService(BLE_BODYNODES_SERVICE_UUID);

static BLECharacteristic sPlayerChara(BLE_BODYNODES_CHARA_PLAYER_UUID, BLERead, CHARA_MAX_LENGTH);
static BLECharacteristic sBodypartChara(BLE_BODYNODES_CHARA_BODYPART_UUID, BLERead, CHARA_MAX_LENGTH);

static BLECharacteristic sOrientationAbsChara(BLE_BODYNODES_CHARA_ORIENTATION_ABS_VALUE_UUID, BLENotify, CHARA_MAX_LENGTH);
static BLECharacteristic sAccelerationRelChara(BLE_BODYNODES_CHARA_ACCELERATION_REL_VALUE_UUID, BLENotify, CHARA_MAX_LENGTH);
static BLECharacteristic sGloveChara(BLE_BODYNODES_CHARA_GLOVE_VALUE_UUID, BLENotify, CHARA_MAX_LENGTH);
static BLECharacteristic sShoeChara(BLE_BODYNODES_CHARA_SHOE_UUID, BLENotify, CHARA_MAX_LENGTH);
static BLECharacteristic sAngularvelocityRelChara("0000CCA7-0000-1000-8000-00805F9B34FB", BLENotify, CHARA_MAX_LENGTH);


// This boolean just forces the Player and Bodypart to be set once
// This is by specifications. The reason is that BLE communication is quite unique in nature and it is just
// more complex to handle changing parts
static bool sPlayerBodypartSet = false;

void BnBLENodeCommunicator_init(){

    pinMode(STATUS_CONNECTION_HMI_LED_P, OUTPUT);
    pinMode(STATUS_CONNECTION_HMI_LED_M, OUTPUT);
    digitalWrite(STATUS_CONNECTION_HMI_LED_P, LOW);
    digitalWrite(STATUS_CONNECTION_HMI_LED_M, LOW);
    if (!BLE.begin()) {
        DEBUG_PRINTLN("BLE failed to initialize!");
    }
    
    BLE.setLocalName("Bodynod0");
    BLE.setAdvertisedService(sBodynodesService);
    
    // Add characteristics to the service
    sBodynodesService.addCharacteristic(sPlayerChara);
    sBodynodesService.addCharacteristic(sBodypartChara);
    sBodynodesService.addCharacteristic(sOrientationAbsChara);
    sBodynodesService.addCharacteristic(sAccelerationRelChara);
    sBodynodesService.addCharacteristic(sGloveChara);
    sBodynodesService.addCharacteristic(sAngularvelocityRelChara);

    
    // Add the service
    BLE.addService(sBodynodesService);
    
    // Start advertising
    BLE.advertise();
    sPlayerBodypartSet = false;
    DEBUG_PRINTLN("BLE service started and advertising.");
}

uint8_t BnBLENodeCommunicator_checkAllOk( uint8_t current_conn_status ){
    BLE.poll();
    if (current_conn_status == CONNECTION_STATUS_NOT_CONNECTED){
        DEBUG_PRINTLN("Not connected");
        delay(1000);
        return CONNECTION_STATUS_WAITING_ACK;
    } else {
        BLEDevice central = BLE.central();
        if( central.connected() ) {
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
            sPlayerChara.setValue(message_json[MESSAGE_PLAYER_TAG].as<String>().c_str());
            sBodypartChara.setValue(message_json[MESSAGE_BODYPART_TAG].as<String>().c_str());
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
            sOrientationAbsChara.setValue(bytes_message, 16);
        } else if(sensortype_str == SENSORTYPE_ACCELERATION_REL_TAG) {
            uint8_t bytes_message[12];
            convertStringArrayToFloatBytes(value_str.c_str(), value_str.length(), bytes_message, 3);
            sAccelerationRelChara.setValue(bytes_message, 12);
        } else if(sensortype_str == SENSORTYPE_GLOVE_TAG) {
            uint8_t bytes_message[9];
            convertStringArrayToUInt8Bytes(value_str.c_str(), value_str.length(), bytes_message, 9);
            sGloveChara.setValue(bytes_message, 9);
        } else if(sensortype_str == SENSORTYPE_SHOE_TAG) {
            //TODO sShoeChara.setValue(message_json[MESSAGE_VALUE_TAG].as<String>().c_str());
        } else if(sensortype_str == "angularvelocity_rel") {
            uint8_t bytes_message[12];
            convertStringArrayToFloatBytes(value_str.c_str(), value_str.length(), bytes_message, 3);
            sAngularvelocityRelChara.setValue(bytes_message, 12);
        } 
    }
    uint8_t num_messages = bnc_messages_list.size();
    for (uint8_t index = 0; index<num_messages; ++index) {
        bnc_messages_list.remove(0);
    }

}

#endif
