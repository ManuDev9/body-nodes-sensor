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
// Board: MicroPro nRF52840  | Adafruit nRF52 -> Adafruit Feather nRF52840 Express

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

////////////// This function is needed to overcome some limitations set by the library, basically mapping is missing pins because we are making use of another board
/// Basically for some pins we want to overpass the variant.cpp file
/// Modified from wiring_digital.c

#include "nrf.h"

void rawDigitalWrite( uint32_t rawPin, uint32_t ulVal )
{
    NRF_GPIO_Type * port = nrf_gpio_pin_port_decode(&rawPin);
    
    switch ( ulVal )
    {
        case LOW:
          port->OUTCLR = (1UL << rawPin);
        break ;
      
        default:
          port->OUTSET = (1UL << rawPin);
        break ;
    }
}


void rawPinMode( uint32_t rawPin, uint32_t ulMode )
{

  NRF_GPIO_Type * port = nrf_gpio_pin_port_decode(&rawPin);

  // Set pin mode according to chapter '22.6.3 I/O Pin Configuration'
  switch ( ulMode )
  {
    case INPUT:
      // Set pin to input mode
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break;

    case INPUT_PULLUP:
      // Set pin to input mode with pull-up resistor enabled
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Pullup      << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break;

    case INPUT_PULLUP_SENSE:
      // Set pin to input mode with pull-up resistor enabled and sense when Low
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Pullup      << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_Low        << GPIO_PIN_CNF_SENSE_Pos);
    break;

    case INPUT_PULLDOWN:
      // Set pin to input mode with pull-down resistor enabled
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Pulldown    << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break;

    case INPUT_PULLDOWN_SENSE:
      // Set pin to input mode with pull-down resistor enabled and sense when High
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Pulldown    << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_High       << GPIO_PIN_CNF_SENSE_Pos);
    break;

    case INPUT_SENSE_HIGH:
      // Set pin to input mode and sense when High
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_High       << GPIO_PIN_CNF_SENSE_Pos);
    break;

    case INPUT_SENSE_LOW:
      // Set pin to input mode and sense when Low
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_Low        << GPIO_PIN_CNF_SENSE_Pos);
    break;

    case OUTPUT_S0S1:
      // Set pin to output mode, sink to standard and source to standard
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Output       << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break;

    case OUTPUT_H0S1:
      // Set pin to output mode, sink to high drive and source to standard
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Output       << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_H0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break;

    case OUTPUT_S0H1:
      // Set pin to output mode, sink to standard and source to high drive
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Output       << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0H1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break;

    case OUTPUT_H0H1:
      // Set pin to output mode, sink to high drive and source to high drive
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Output       << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_H0H1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break;

    case OUTPUT_D0S1:
      // Set pin to output mode, sink to disconnect and source to standard
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Output       << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_D0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break;

    case OUTPUT_D0H1:
      // Set pin to output mode, sink to disconnect and source to high drive
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Output       << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_D0H1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break;

    case OUTPUT_S0D1:
      // Set pin to output mode, sink to standard and source to disconnect
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Output       << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0D1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break;

    case OUTPUT_H0D1:
      // Set pin to output mode, sink to high drive and source to disconnect
      port->PIN_CNF[rawPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Output       << GPIO_PIN_CNF_DIR_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_DRIVE_H0D1       << GPIO_PIN_CNF_DRIVE_Pos)
                           | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break;

    default:
      // do nothing
    break ;
  }
}


int rawDigitalRead( uint32_t rawPin )
{    
    NRF_GPIO_Type * port = nrf_gpio_pin_port_decode(&rawPin);
    uint32_t const bm = (1UL << rawPin);
    
    // Return bit in OUT or IN depending on configured direction
    return (bm  & ((port->DIR & bm) ? port->OUT : port->IN)) ? 1 : 0;
}

//////////////


#ifdef BLE_COMMUNICATION

#include <bluefruit.h>

#define CHARA_MAX_LENGTH 20

static BLEService sBodynodesService(BLE_BODYNODES_SERVICE_UUID);

static BLECharacteristic sPlayerChara(BLE_BODYNODES_CHARA_PLAYER_UUID);
static BLECharacteristic sBodypartChara(BLE_BODYNODES_CHARA_BODYPART_UUID);

static BLECharacteristic sOrientationAbsChara(BLE_BODYNODES_CHARA_ORIENTATION_ABS_VALUE_UUID);
static BLECharacteristic sAccelerationRelChara(BLE_BODYNODES_CHARA_ACCELERATION_REL_VALUE_UUID);
static BLECharacteristic sGloveChara(BLE_BODYNODES_CHARA_GLOVE_VALUE_UUID);
static BLECharacteristic sShoeChara(BLE_BODYNODES_CHARA_SHOE_UUID);

static BLECharacteristic sAngularvelocityRelChara("0000CCA7-0000-1000-8000-00805F9B34FB");

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
    DEBUG_PRINTLN("Connected");
    sIsConnected = true;

}

// Callback when disconnected
void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
    DEBUG_PRINT("Disconnected, reason: ");
    DEBUG_PRINTLN(reason);

    // Restart advertising after disconnection
    startAdv();
    sIsConnected = false;
}

void BnBLENodeCommunicator_init(){
    
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
    sPlayerChara.begin( );

    sBodypartChara.setProperties( CHR_PROPS_READ ); // Read properties
    sBodypartChara.setPermission( SECMODE_OPEN, SECMODE_NO_ACCESS );
    sBodypartChara.begin( );

    sOrientationAbsChara.setProperties( CHR_PROPS_NOTIFY ); // Notify properties  
    sOrientationAbsChara.setPermission( SECMODE_OPEN, SECMODE_NO_ACCESS ); 
    sOrientationAbsChara.setFixedLen( 16 );  
    sOrientationAbsChara.begin( );

    sAccelerationRelChara.setProperties( CHR_PROPS_NOTIFY ); // Notify properties  
    sAccelerationRelChara.setPermission( SECMODE_OPEN, SECMODE_NO_ACCESS ); 
    sAccelerationRelChara.setFixedLen( 12 );  
    sAccelerationRelChara.begin( );

    sGloveChara.setProperties( CHR_PROPS_NOTIFY ); // Notify properties  
    sGloveChara.setPermission( SECMODE_OPEN, SECMODE_NO_ACCESS ); 
    sGloveChara.setFixedLen( 9 );  
    sGloveChara.begin( );

    sAngularvelocityRelChara.setProperties( CHR_PROPS_NOTIFY ); // Notify properties  
    sAngularvelocityRelChara.setPermission( SECMODE_OPEN, SECMODE_NO_ACCESS ); 
    sAngularvelocityRelChara.setFixedLen( 12 );  
    sAngularvelocityRelChara.begin( );

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
            //convertStringArrayToFloatBytes(value_str.c_str(), value_str.length(), bytes_message, 4);
            //sOrientationAbsChara.notify(&bytes_message, 16);
        } else if(sensortype_str == SENSORTYPE_ACCELERATION_REL_TAG) {
            uint8_t bytes_message[12];
            convertStringArrayToFloatBytes(value_str.c_str(), value_str.length(), bytes_message, 3);
            sAccelerationRelChara.notify(&bytes_message, 12);
        } else if(sensortype_str == SENSORTYPE_GLOVE_TAG) {
            uint8_t bytes_message[9];
            convertStringArrayToUInt8Bytes(value_str.c_str(), value_str.length(), bytes_message, 9);
            sGloveChara.notify(&bytes_message, 9);
        } else if(sensortype_str == SENSORTYPE_SHOE_TAG) {
            //TODO sShoeChara.write(message_json[MESSAGE_VALUE_TAG].as<String>().c_str());
            //TODO sShoeChara.notify(&bytes_message, 1);
        } else if(sensortype_str == "angularvelocity_rel") {
            uint8_t bytes_message[12];
            convertStringArrayToFloatBytes(value_str.c_str(), value_str.length(), bytes_message, 3);
            sAngularvelocityRelChara.notify(&bytes_message, 12);
        } 
    }
    uint8_t num_messages = bnc_messages_list.size();
    for (uint8_t index = 0; index<num_messages; ++index) {
        bnc_messages_list.remove(0);
    }

}

#endif
