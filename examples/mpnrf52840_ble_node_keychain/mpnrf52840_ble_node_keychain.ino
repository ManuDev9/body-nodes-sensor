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

// This is a custom implementation of a BLE node of the BLE Bodynodes Keychain V1.0
// It has a button acting a sensor glove

#include "BnNodeSpecific.h"
#include "BnArduinoUtils.h"
#include "BnDatatypes.h"

#ifdef BLE_COMMUNICATION
#include "BnBLENodeCommunicator.h"
BnBLENodeCommunicator mCommunicator;
#endif // BLE_COMMUNICATION

#ifdef ORIENTATION_ABS_SENSOR

// ORIENTATION_ABS_SENSOR_HEADER //
#include "BnOrientationAbsSensorFusion_MPU6050.h"
BnOrientationAbsSensor mOASensor;
float mLastSensorData_OA[4] = {0 ,0 ,0 ,0};
float mBigDiff_OA[4] = {BIG_QUAT_DIFF ,BIG_QUAT_DIFF ,BIG_QUAT_DIFF ,BIG_QUAT_DIFF};

float mLastSensorData_Gy[3] = {0 ,0 ,0};
float mBigDiff_Gy[3] = {0.1 ,0.1 ,0.1};

#endif // ORIENTATION_ABS_SENSOR

int mLastSensorData_G[9] = {0 ,0 ,0 ,0, 0, 0 ,0 ,0 ,0};
int mBigDiff_G[9] = {BIG_ANGLE_DIFF ,BIG_ANGLE_DIFF ,BIG_ANGLE_DIFF ,BIG_ANGLE_DIFF, BIG_ANGLE_DIFF, 0, 0, 0, 0};

#ifdef HAPTIC_ACTUATOR_ON_BOARD
#include "BnHapticActuator.h"
BnHapticActuator mHapticActuator;
#endif // HAPTIC_ACTUATOR_ON_BOARD

String mPlayerName;
String mBodypartName;

//#define BUTTON_LEFT_PIN (32+13) // Keychain 0

#define BUTTON_LEFT_PIN (32+4)
#define BUTTON_RIGHT_PIN (10)

template<typename T>
bool bigChanges(T values[], T prev_values[], uint8_t num_values, T big_difference[]) {
    bool somethingChanged=false;
    
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

    BnPersMemory::init();

#ifdef HAPTIC_ACTUATOR_ON_BOARD
    mHapticActuator.init();
#endif // HAPTIC_ACTUATOR_ON_BOARD

#ifdef ORIENTATION_ABS_SENSOR
    mOASensor.init();
#endif // ORIENTATION_ABS_SENSOR

    mCommunicator.init();

    mPlayerName = "1"; // BnPersMemory::getValue(MEMORY_PLAYER_TAG);
    mBodypartName = "hand_left"; // BnPersMemory::getValue(MEMORY_BODYPART_TAG);

    rawPinMode(BUTTON_LEFT_PIN, INPUT_PULLDOWN);
    rawPinMode(BUTTON_RIGHT_PIN, INPUT_PULLDOWN);

}

void loop() {

    if(mCommunicator.checkAllOk()){
#ifdef ORIENTATION_ABS_SENSOR
        if(!mOASensor.isCalibrated()){
            // You can decide to return
        }

        if(mOASensor.isEnabled() && mOASensor.checkAllOk()) {
            float values[4] = {0, 0, 0, 0};
            mOASensor.getData().getValues(values);
            if(bigChanges(values, mLastSensorData_OA, 4, mBigDiff_OA)) {
                StaticJsonDocument<MAX_MESSAGE_BYTES> message_doc;
                JsonObject message = message_doc.to<JsonObject>();;
                message["player"] = mPlayerName;
                message["bodypart"] = mBodypartName;
                message["sensortype"] = mOASensor.getType();
                for(uint8_t count=0; count<4;++count){
                    message["value"].add(values[count]);
                    mLastSensorData_OA[count] = values[count];
                }

                //DEBUG_PRINT("message = ");
                //String output;
                //serializeJson(message, output);
                //DEBUG_PRINTLN(output);
                mCommunicator.addMessage(message);
            }

            float valuesg[3] = {0, 0, 0};
            mOASensor.getGyro(valuesg);
            if(bigChanges(valuesg, mLastSensorData_Gy, 3, mBigDiff_Gy)) {
                StaticJsonDocument<MAX_MESSAGE_BYTES> message_doc;
                JsonObject message = message_doc.to<JsonObject>();;
                message["player"] = mPlayerName;
                message["bodypart"] = mBodypartName;
                message["sensortype"] = SENSORTYPE_ANGULARVELOCITY_REL_TAG;
                for(uint8_t count=0; count<3;++count){
                    message["value"].add(valuesg[count]);
                    mLastSensorData_Gy[count] = valuesg[count];
                }

                //DEBUG_PRINT("message = ");
                //String output;
                //serializeJson(message, output);
                //DEBUG_PRINTLN(output);
                mCommunicator.addMessage(message);
            }
        }

#endif // ORIENTATION_ABS_SENSOR

        // Check button
        int buttonLeft = rawDigitalRead( BUTTON_LEFT_PIN );
        int buttonRight = rawDigitalRead( BUTTON_RIGHT_PIN );
        
        int values[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        if (buttonLeft > 0){
            values[8] = 1;
        }
        if (buttonRight > 0){
            values[7] = 1;
        }
        
        if(bigChanges(values, mLastSensorData_G, 9, mBigDiff_G)) {
            StaticJsonDocument<MAX_MESSAGE_BYTES> message_doc;
            JsonObject message = message_doc.to<JsonObject>();;
            message["player"] = mPlayerName;
            message["bodypart"] = mBodypartName;
            message["sensortype"] = SENSORTYPE_GLOVE_TAG;
            for(uint8_t count=0; count<9;++count){
                message["value"].add(values[count]);
                mLastSensorData_G[count] = values[count];
            }
            mCommunicator.addMessage(message);
        }
        
        mCommunicator.sendAllMessages();

        StaticJsonDocument<MAX_ACTION_BYTES> actions_doc;
        JsonArray actions = actions_doc.to<JsonArray>();
        mCommunicator.getActions(actions);

        for (JsonObject action : actions) {
            String actionPlayer = action[ACTION_PLAYER_TAG].as<String>();
            String actionBodypart = action[ACTION_BODYPART_TAG].as<String>();
            if(actionPlayer != mPlayerName) {
                DEBUG_PRINTLN("Wrong player in the action");
                continue;
            }
            if(actionBodypart != mBodypartName) {
                DEBUG_PRINTLN("Wrong bodypart in the action");
                continue;
            }
            String actionType = action[ACTION_TYPE_TAG].as<String>();
            if(actionType == ACTION_TYPE_HAPTIC_TAG) {
#ifdef HAPTIC_ACTUATOR_ON_BOARD
                mHapticActuator.setAction(action);
#endif // HAPTIC_ACTUATOR_ON_BOARD
            } else if(actionType == ACTION_TYPE_ENABLESENSOR_TAG) {
                String actionSensorType = action[ACTION_ENABLESENSOR_SENSORTYPE_TAG].as<String>();
                if(actionSensorType == SENSORTYPE_ORIENTATION_ABS_TAG) {
                    //DEBUG_PRINT("Setting enabled = ");
                    //DEBUG_PRINTLN(action[ACTION_ENABLESENSOR_ENABLE_TAG].as<bool>());
#ifdef ORIENTATION_ABS_SENSOR
                    mOASensor.setEnable(action[ACTION_ENABLESENSOR_ENABLE_TAG].as<bool>());
#endif /*ORIENTATION_ABS_SENSOR*/
                }
            } else if(actionType == ACTION_TYPE_SETPLAYER_TAG) {
                mPlayerName = action[ACTION_SETPLAYER_NEWPLAYER_TAG].as<String>();
                BnPersMemory::setValue(MEMORY_PLAYER_TAG, mPlayerName);
            } else if(actionType == ACTION_TYPE_SETBODYPART_TAG) {
                mBodypartName = action[ACTION_SETBODYPART_NEWBODYPART_TAG].as<String>();
                BnPersMemory::setValue(MEMORY_BODYPART_TAG, mBodypartName);
            }
        }
    }

#ifdef HAPTIC_ACTUATOR_ON_BOARD
    mHapticActuator.performAction();
#endif // HAPTIC_ACTUATOR_ON_BOARD
    //delay(SENSOR_READ_INTERVAL_MS);
}
