/**
* MIT License
*
* Copyright (c) 2021-2025 Manuel Bottini
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

#include "BnOrientationAbsSensor.h"

#ifdef __BN_ORIENTATION_ABS_SENSOR_H__

void BnOrientationAbsSensor::init(){
    s_enabled = true;

    s_sensorInit=false;
    s_lastReadSensorTime=millis();
    s_sensorReconnectionTime=millis();
    /* Initialise the sensor */
    if(s_isensor.init()) {
        s_sensorInit=true;
        s_firstZeros=true;
    }
}


bool BnOrientationAbsSensor::checkAllOk(){
    if(!s_sensorInit){
        if(millis()-s_sensorReconnectionTime<5000){
            return false;
        }
        DEBUG_PRINTLN("Sensor not connected");
        s_sensorReconnectionTime=millis();
        if(s_isensor.init()) {
            s_firstZeros=true;
            s_sensorInit=true;
            return true;
        } else {
            return false;
        }
    }
    if(millis()-s_lastReadSensorTime<SENSOR_READ_INTERVAL_MS){
        return false;
    }


    float svalues[4];
    if( !s_isensor.getData(svalues, ISENSOR_DATATYPE_ABSOLUTEORIENTATION) ) {
        return false;
    }
    
    float tvalues[4];
    realignAxis(svalues, tvalues);

    //I noticed that just before after a disconnection the getEvent returns 0 0 0 and THEN blocks. So I just check before the next call to getEvent blocks everything
    if (tvalues[0] != 0 || tvalues[1]!=0 || tvalues[2]!=0 || tvalues[3]!=0) {
        s_firstZeros=false;
    }

    if (tvalues[0] == 0 && tvalues[1]==0 && tvalues[2]==0 && tvalues[3]==0 && !s_firstZeros){
        DEBUG_PRINTLN("Sensor might have gotten disconnected!");
        s_isensor.setStatus(SENSOR_STATUS_NOT_ACCESSIBLE);
        return false;
    }

    s_values[0] = tvalues[0];
    s_values[1] = tvalues[1];
    s_values[2] = tvalues[2];
    s_values[3] = tvalues[3];

    return true;
}

bool BnOrientationAbsSensor::isCalibrated(){
    return s_isensor.isCalibrated();
}

BnSensorData BnOrientationAbsSensor::getData(){
    s_lastReadSensorTime=millis();
    /*
    DEBUG_PRINT("values = ");
    DEBUG_PRINT(s_values[0]);
    DEBUG_PRINT(", ");
    DEBUG_PRINT(s_values[1]);
    DEBUG_PRINT(", ");
    DEBUG_PRINT(s_values[2]);
    DEBUG_PRINT(", ");
    DEBUG_PRINTLN(s_values[3]);
    */
    BnSensorData sensorData;
    sensorData.setValues(s_values, SENSORTYPE_ORIENTATION_ABS_TAG);
    return sensorData;
}

BnType BnOrientationAbsSensor::getType(){
    return SENSORTYPE_ORIENTATION_ABS_TAG;
}

void BnOrientationAbsSensor::setEnable(bool enable_status){
    s_enabled = enable_status;
}

bool BnOrientationAbsSensor::isEnabled(){
    return s_enabled;
}

void BnOrientationAbsSensor::realignAxis(float values[], float revalues[]){

    revalues[0] = MUL_AXIS_W_ORIE * values[OUT_AXIS_W_ORIE];
    revalues[1] = MUL_AXIS_X_ORIE * values[OUT_AXIS_X_ORIE];
    revalues[2] = MUL_AXIS_Y_ORIE * values[OUT_AXIS_Y_ORIE];
    revalues[3] = MUL_AXIS_Z_ORIE * values[OUT_AXIS_Z_ORIE];
}

#endif /*__BN_ORIENTATION_ABS_SENSOR_H__*/
