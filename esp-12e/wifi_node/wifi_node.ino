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

#include "basics.h"
#include "sensor.h"
#include "wifi_node_communicator.h"
#include "actuator.h"
#include "commons.h"

Actuator mActuator;
WifiNodeCommunicator mCommunicator;
Sensor mSensor;
String mPlayerName;
String mBodypartName;

float mLastSensorData_OA[4] = {0 ,0 ,0 ,0};
float mBigDiff_OA[4] = {BIG_QUAT_DIFF ,BIG_QUAT_DIFF ,BIG_QUAT_DIFF ,BIG_QUAT_DIFF};

template<typename T>
bool bigChanges(T values[], T prev_values[], uint8_t num_values, T big_difference[]) {
  bool somethingChanged=false;

  bool prev_allzeros = true;
  for(uint8_t index = 0; index<num_values;++index){
    if( prev_values[index] != 0 ){
      prev_allzeros = false;
      break;
    }
  }
  if(prev_allzeros){
    // There is not prev data yet
    return true;
  }
  
  for(uint8_t index = 0; index<num_values;++index){
    if( values[index] < prev_values[index]-big_difference[index] || prev_values[index]+big_difference[index] < values[index] ){
      somethingChanged=true;
      break;
    }
  }
  
  return somethingChanged;
}


void setup() {  
  //Initialize the serial and wait for the port to open
  Serial.begin(921600);

  PersMemory::init();
  mActuator.init();
  mSensor.init();
  mCommunicator.init();

  mPlayerName = PersMemory::getValue(ACTION_PLAYER_TAG);
  mBodypartName = PersMemory::getValue(ACTION_BODYPART_TAG);
}

void loop() {
  if(!mSensor.checkAllOk()){
    return;
  }  
  if(!mSensor.isCalibrated()){
    // You can decide to return
  }
  if(mCommunicator.checkAllOk()){
    float values[4] = {0, 0, 0, 0};
    if(mSensor.isEnabled()) {
      mSensor.getData(values);
      if(values[0] != 0 || values[1] != 0 || values[2] != 0 || values[3] != 0){
        if(bigChanges(values, mLastSensorData_OA, 4, mBigDiff_OA)) {
          StaticJsonDocument<MAX_MESSAGE_BYTES> message_doc;
          JsonObject message = message_doc.to<JsonObject>();;
          message["player"] = mPlayerName;
          message["bodypart"] = mBodypartName;
          message["sensortype"] = mSensor.getType();
          message["value"].add(values[0]);
          message["value"].add(values[1]);
          message["value"].add(values[2]);
          message["value"].add(values[3]);
  
          //DEBUG_PRINT("message = ");
          //String output;
          //serializeJson(message, output);
          //DEBUG_PRINTLN(output);
  
          mCommunicator.addMessage(message);
          mCommunicator.sendAllMessages();
          mLastSensorData_OA[0] = values[0];
          mLastSensorData_OA[1] = values[1];
          mLastSensorData_OA[2] = values[2];
          mLastSensorData_OA[3] = values[3];
        }
      }
    }
    StaticJsonDocument<MAX_ACTION_BYTES> actions_doc;
    JsonArray actions = actions_doc.to<JsonArray>();
    mCommunicator.getActions(actions);

    // TODO: SetBodypart, SetPlayer, EnableSensor, SetWifi
    for (JsonObject action : actions) {
      String actionPlayer = action[ACTION_PLAYER_TAG].as<String>();
      String actionBodypart = action[ACTION_BODYPART_TAG].as<String>();
      if( actionPlayer != mPlayerName && actionBodypart != mBodypartName ) {
        continue;
      }
      String actionType = action[ACTION_TYPE_TAG].as<String>();
      if(actionType == mActuator.getType()) {
        mActuator.setAction(action);
      } else if(actionType == ACTION_TYPE_ENABLESENSOR_TAG) {
        String actionSensorType = action[ACTION_ENABLESENSOR_SENSORTYPE_TAG].as<String>();
        if(actionSensorType == mSensor.getType()) {
          //DEBUG_PRINT("Setting enabled = ");
          //DEBUG_PRINTLN(action[ACTION_ENABLESENSOR_ENABLE_TAG].as<bool>());
          mSensor.setEnable(action[ACTION_ENABLESENSOR_ENABLE_TAG].as<bool>());
        }
      } else if(actionType == ACTION_TYPE_SETPLAYER_TAG) {
        mPlayerName = action[ACTION_SETPLAYER_NEWPLAYER_TAG].as<String>();
        PersMemory::setValue(ACTION_PLAYER_TAG, mPlayerName);
      } else if(actionType == ACTION_TYPE_SETBODYPART_TAG) {
        mBodypartName = action[ACTION_SETBODYPART_NEWBODYPART_TAG].as<String>();
        PersMemory::setValue(ACTION_BODYPART_TAG, mBodypartName);
      } else if(actionType == ACTION_TYPE_SETWIFI_TAG) {
        mCommunicator.setConnectionParams(action);
        mCommunicator.init();
      }
    }
  }
  mActuator.performAction();
}
