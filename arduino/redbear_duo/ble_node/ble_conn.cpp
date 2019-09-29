/**
* MIT License
* 
* Copyright (c) 2019 Manuel Bottini
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

#include "ble_conn.h"

bool mIsConnected;
bool mDataToSend;
bool mDataReceived;

static uint8_t serviceOrientation_uuid[16] =
  { 0x00,0x00,0xcc,0xc0,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };

// Characteristics 128-bits UUID
#if NODE_BODY_PART == BODY_HEAD_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xc1,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#elif NODE_BODY_PART == BODY_HAND_LEFT_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xc2,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#elif NODE_BODY_PART == BODY_FOREARM_LEFT_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xc3,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#elif NODE_BODY_PART == BODY_UPPERARM_LEFT_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xc4,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#elif NODE_BODY_PART == BODY_BODY_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xc5,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#elif NODE_BODY_PART == BODY_FOREARM_RIGHT_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xc6,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#elif NODE_BODY_PART == BODY_UPPERARM_RIGHT_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xc7,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#elif NODE_BODY_PART == BODY_HAND_RIGHT_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xc8,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#elif NODE_BODY_PART == BODY_LOWERLEG_LEFT_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xc9,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#elif NODE_BODY_PART == BODY_UPPERLEG_LEFT_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xca,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#elif NODE_BODY_PART == BODY_FOOT_LEFT_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xcb,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#elif NODE_BODY_PART == BODY_LOWERLEG_RIGHT_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xcc,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#elif NODE_BODY_PART == BODY_UPPERLEG_RIGHT_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xcd,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#elif NODE_BODY_PART == BODY_FOOT_RIGHT_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xce,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#elif NODE_BODY_PART == BODY_UNTAGGED_INT
  static uint8_t characteristicOrientation_uuid[16] =
    { 0x00,0x00,0xcc,0xcf,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };
#endif


static uint8_t serviceAction_uuid[16] =
  { 0x00,0x00,0xcc,0xe0,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };

static uint8_t characteristicAction_uuid[16] =
  { 0x00,0x00,0xcc,0xe1,  0x00,0x00,  0x10,0x00,  0x80,0x00,  0x00,0x80,0x5f,0x9b,0x34,0xfb };


static uint8_t appearance[2] = { 
  LOW_BYTE(BLE_PERIPHERAL_APPEARANCE), 
  HIGH_BYTE(BLE_PERIPHERAL_APPEARANCE) 
};

static uint8_t  change[4] = {
  0x00, 0x00, 0xFF, 0xFF
};



// Timer task.
static btstack_timer_source_t characteristicOrientation_timer;

static uint8_t  conn_param[8] = {
  LOW_BYTE(BLE_MIN_INTERVAL), HIGH_BYTE(BLE_MIN_INTERVAL), 
  LOW_BYTE(BLE_MAX_INTERVAL), HIGH_BYTE(BLE_MAX_INTERVAL), 
  LOW_BYTE(SLAVE_LATENCY), HIGH_BYTE(SLAVE_LATENCY), 
  LOW_BYTE(CONN_SUPERVISION_TIMEOUT), HIGH_BYTE(CONN_SUPERVISION_TIMEOUT)
};

static advParams_t adv_params = {
  .adv_int_min   = 0x0030,
  .adv_int_max   = 0x0030,
  .adv_type      = BLE_GAP_ADV_TYPE_ADV_IND,
  .dir_addr_type = BLE_GAP_ADDR_TYPE_PUBLIC,
  .dir_addr      = {0,0,0,0,0,0},
  .channel_map   = BLE_GAP_ADV_CHANNEL_MAP_ALL,
  .filter_policy = BLE_GAP_ADV_FP_ANY
};


// ble peripheral advertising data
static uint8_t adv_data[] = {
  0x02,
  BLE_GAP_AD_TYPE_FLAGS,
  BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE,   
  
  0x11,
  BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE,
  0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x00, 0x00, 0x3d, 0x71 
};

// ble peripheral scan respond data
static uint8_t scan_response[] = {
  0x09,
  BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME,
  'B', 'L',  'E', '_', 'N', 'o', 'd', 'e'
};

// Characteristic value handle
static uint16_t characteristicOrientation_handle = 0x0000;
static uint16_t characteristicAction_handle = 0x0000;
// Buffer of characterisitc value.
static uint8_t characteristicOrientation_data[CHARACTERISTIC_ORI_MAX_LEN] = { 0x00 };
static uint8_t characteristicAction_data[CHARACTERISTIC_ACT_MAX_LEN] = { 0x00 };

static void characteristicOrientationSensor_notify(btstack_timer_source_t *ts) {
  if(mDataToSend){
    ble.sendNotify(characteristicOrientation_handle, characteristicOrientation_data, CHARACTERISTIC_ORI_MAX_LEN);
    mDataToSend = false;
  }
  // Restart timer.
  ble.setTimer(ts, SENSOR_READ_INTERVAL_MS);
  ble.addTimer(ts);
}

void initStatusConnectionHMI(){
  pinMode(STATUS_CONNECTION_HMI_LED_P, OUTPUT);
  pinMode(STATUS_CONNECTION_HMI_LED_M, OUTPUT);
  digitalWrite(STATUS_CONNECTION_HMI_LED_P, LOW);
  digitalWrite(STATUS_CONNECTION_HMI_LED_M, LOW);
}

void setStatusConnectionHMI_ON(){
  mIsConnected = true;
  digitalWrite(STATUS_CONNECTION_HMI_LED_P, HIGH);
}

void setStatusConnectionHMI_OFF(){
  mIsConnected = false;
  digitalWrite(STATUS_CONNECTION_HMI_LED_P, LOW);
}

/**
 * @brief Connect handle.
 *
 * @param[in]  status   BLE_STATUS_CONNECTION_ERROR or BLE_STATUS_OK.
 * @param[in]  handle   Connect handle.
 *
 * @retval None
 */
void deviceConnectedCallback(BLEStatus_t status, uint16_t handle) {
  switch (status) {
    case BLE_STATUS_OK:
      setStatusConnectionHMI_ON();
      //DEBUG_PRINT("Device connected!");
      break;
    default: break;
  }
}

void deviceDisconnectedCallback(uint16_t handle) {
  setStatusConnectionHMI_OFF();
  //DEBUG_PRINT("DmIsConnected.");
}

/**
 * @brief Callback for reading event.
 *
 * @note  If characteristic contains client characteristic configuration,then client characteristic configration handle is value_handle+1.
 *        Now can't add user_descriptor.
 *
 * @param[in]  value_handle    
 * @param[in]  buffer 
 * @param[in]  buffer_size    Ignore it.
 *
 * @retval  Length of current attribute value.
 */
uint16_t gattReadCallback(uint16_t value_handle, uint8_t * buffer, uint16_t buffer_size) {   
  uint8_t characteristic_len = 0;

  DEBUG_PRINT("Read value handler: ");
  DEBUG_PRINTLN_HEX(value_handle);

  if (characteristicOrientation_handle == value_handle) {   // Characteristic value handle.
    DEBUG_PRINTLN("characteristicOrientation_data read: sending bodypart");
    memcpy(buffer, NODE_BODYPART_FULL_TAG, NODE_BODYPART_FULL_LEN);
    characteristic_len = NODE_BODYPART_FULL_LEN;
  }
  else if (characteristicOrientation_handle+1 == value_handle) {   // Client Characteristic Configuration Descriptor Handle.
    DEBUG_PRINTLN("characteristicOrientationSensor_descriptor cccd read:");
    uint8_t buf[3] = { 's', 'u', 'p' };
    memcpy(buffer, buf, 3);
    characteristic_len = 3;
  } else if(characteristicAction_handle == value_handle){
    DEBUG_PRINTLN("characteristicAction_handle read: sending action");
    memcpy(buffer, NODE_ACTION_FULL_TAG, NODE_ACTION_FULL_LEN);
    characteristic_len = NODE_ACTION_FULL_LEN;
  } else if(characteristicAction_handle+1 == value_handle){
    DEBUG_PRINTLN("characteristicAction_descriptor cccd read:");
    uint8_t buf[3] = { 's', 'u', 'p' };
    memcpy(buffer, buf, 3);
    characteristic_len = 3;
  }
  return characteristic_len;
}

/**
 * @brief Callback for writting event.
 *
 * @param[in]  value_handle  
 * @param[in]  *buffer       The buffer pointer of writting data.
 * @param[in]  size          The length of writting data.   
 *
 * @retval 
 */
int gattWriteCallback(uint16_t value_handle, uint8_t *buffer, uint16_t size) {

  if (characteristicAction_handle == value_handle)
  {
    
    DEBUG_PRINT("Characteristic Action write value: ");
    memcpy(characteristicAction_data, buffer, size);
    mDataReceived = true;
    for (uint8_t index = 0; index < size; index++) {
      DEBUG_PRINT_HEX(characteristicAction_data[index]);
      DEBUG_PRINT(" ");
    }
    DEBUG_PRINTLN(" ");
  }
  else if (characteristicAction_handle+1 == value_handle)
  {
    DEBUG_PRINT("Characteristic Action +1 write value: ");
    for (uint8_t index = 0; index < size; index++) {
      DEBUG_PRINT_HEX(buffer[index]);
      DEBUG_PRINT(" ");
    }
    DEBUG_PRINTLN(" ");
  }
  return 0;
}

void initBLE(){
  mIsConnected = false;
  mDataToSend = false;
  mDataReceived = false;
  
  ble.init();

  // Register ble callback functions.
  ble.onConnectedCallback(deviceConnectedCallback);
  ble.onDisconnectedCallback(deviceDisconnectedCallback);
  ble.onDataReadCallback(gattReadCallback);
  ble.onDataWriteCallback(gattWriteCallback);

  // Add GAP service and characteristics
  ble.addService(BLE_UUID_GAP);
  ble.addCharacteristic(BLE_UUID_GAP_CHARACTERISTIC_DEVICE_NAME, ATT_PROPERTY_READ, (uint8_t*)NODE_DEVICE_NAME_TAG, sizeof(NODE_DEVICE_NAME_TAG));
  ble.addCharacteristic(BLE_UUID_GAP_CHARACTERISTIC_APPEARANCE, ATT_PROPERTY_READ, appearance, sizeof(appearance));
  ble.addCharacteristic(BLE_UUID_GAP_CHARACTERISTIC_PPCP, ATT_PROPERTY_READ, conn_param, sizeof(conn_param));

  // Add GATT service and characteristics
  ble.addService(BLE_UUID_GATT);
  ble.addCharacteristic(BLE_UUID_GATT_CHARACTERISTIC_SERVICE_CHANGED, ATT_PROPERTY_INDICATE, change, sizeof(change));

  // Add oritentation service and characteristic
  ble.addService(serviceOrientation_uuid);
  characteristicOrientation_handle = ble.addCharacteristicDynamic(characteristicOrientation_uuid, ATT_PROPERTY_READ|ATT_PROPERTY_NOTIFY, characteristicOrientation_data, CHARACTERISTIC_ORI_MAX_LEN);
  
  // Add action service and characteristic
  ble.addService(serviceAction_uuid);
  characteristicAction_handle = ble.addCharacteristicDynamic(characteristicAction_uuid, ATT_PROPERTY_READ|ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, characteristicAction_data, CHARACTERISTIC_ACT_MAX_LEN);

  // Set ble advertising parameters
  ble.setAdvertisementParams(&adv_params);

  // Set ble advertising and scan respond data
  ble.setAdvertisementData(sizeof(adv_data), adv_data);
  ble.setScanResponseData(sizeof(scan_response), scan_response);
  
  // Start advertising.
  ble.startAdvertising();
  DEBUG_PRINTLN("ble start advertising.");  

  // set one-shot timer
  characteristicOrientation_timer.process = &characteristicOrientationSensor_notify;
  ble.setTimer(&characteristicOrientation_timer, SENSOR_READ_INTERVAL_MS);
  ble.addTimer(&characteristicOrientation_timer);
}


void sendOrientationValueQuat(imu::Quaternion quat){

  unsigned char values[DIMENSIONS_OF_QUAT*BYTES_FLOAT_QUAT];
  float_converter s;

  ///First 4 chars are W
  //DEBUG_PRINT("quat.w() = ");
  //DEBUG_PRINTLN(quat.w());
  s.number = quat.w();

  //DEBUG_PRINT("s = ");
  for(int c_b=0;c_b<BYTES_FLOAT_QUAT;c_b++){
    values[c_b] = s.bytes[c_b];
  }


  ///Second 4 chars are X
  s.number = quat.x();
  for(int c_b=0;c_b<BYTES_FLOAT_QUAT;c_b++){
    values[BYTES_FLOAT_QUAT + c_b] = s.bytes[c_b];
  }

  ///Third 4 chars are Y
  s.number = quat.y();
  for(int c_b=0;c_b<BYTES_FLOAT_QUAT;c_b++){
    values[2*BYTES_FLOAT_QUAT + c_b] = s.bytes[c_b];
  }

  ///Fourth 4 chars are Z
  s.number = quat.z();
  for(int c_b=0;c_b<BYTES_FLOAT_QUAT;c_b++){
    values[3*BYTES_FLOAT_QUAT + c_b] = s.bytes[c_b];
  }
  
  DEBUG_PRINT("values = ");

  for(int i=0;i<16;i++){
    DEBUG_PRINT_HEX(values[i]);
    DEBUG_PRINT("  ");
  }
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("");
  memcpy(characteristicOrientation_data, values, CHARACTERISTIC_ORI_MAX_LEN);
  mDataToSend = true;
}


Action checkActionBLE(){
  Action action;
  action.type = ACTION_NOPE_INT;
  action.duration_ms = 0;
  action.strength = 0;
  //Here an example 00010064ff
  if(mDataReceived){
    action.type = characteristicAction_data[0] << 8 | characteristicAction_data[1];
    action.duration_ms = characteristicAction_data[2] << 8 | characteristicAction_data[3];
    action.strength = characteristicAction_data[4];
    mDataReceived = false;
  }
  return action;
}


bool isConnOK(){
  return mIsConnected;  
}
