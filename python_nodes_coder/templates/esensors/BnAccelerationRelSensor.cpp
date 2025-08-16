/**
* MIT License
* 
* Copyright (c) 2024-2025 Manuel Bottini
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

#include "BnAccelerationRelSensor.h"

#ifdef __BN_ACCELERATION_ABS_SENSOR_H__

#include "BnDatatypes.h"

void BnAccelerationRelSensor::init(){
    s_enabled = true;

    s_sensorInit=false;
    s_lastReadSensorTime=millis();
    s_sensorReconnectionTime=millis();
    /* Initialise the sensor */
    if(s_isensor.init()) {
         setStatus(SENSOR_STATUS_WORKING);
    } else {
         setStatus(SENSOR_STATUS_NOT_ACCESSIBLE);
    }
}

void BnAccelerationRelSensor::setStatus(int sensor_status){
    if(sensor_status == SENSOR_STATUS_NOT_ACCESSIBLE){
        s_sensorInit=false;
        DEBUG_PRINTLN("Ooops, no isensor detected ... Check your wiring or I2C ADDR!");
    } else if(sensor_status == SENSOR_STATUS_CALIBRATING) {
    } else if(sensor_status == SENSOR_STATUS_WORKING) {
        s_sensorInit=true;
    }
}

bool BnAccelerationRelSensor::checkAllOk(){
    if(!s_sensorInit){
        if(millis()-s_sensorReconnectionTime<5000){
            return false;
        }
        DEBUG_PRINTLN("Sensor not connected");
        s_sensorReconnectionTime=millis();
        if (s_isensor.init()) {
            setStatus(SENSOR_STATUS_WORKING);
            return true;
        } else {
            setStatus(SENSOR_STATUS_NOT_ACCESSIBLE);
            return false;
        }
    }
    if(millis()-s_lastReadSensorTime<SENSOR_READ_INTERVAL_MS){
        return false;
    }

    float acc_values[3];
    if( !s_isensor.getData(acc_values, ISENSOR_DATATYPE_ACCELEROMETER) ) {
        return false;
    }

    float accel1_vals[] = { acc_values[0], acc_values[1], acc_values[2] };

    float tvalues[3];
    realignAxis(accel1_vals, tvalues);

    s_values[0] = tvalues[0];
    s_values[1] = tvalues[1];
    s_values[2] = tvalues[2];

    return true;
}

bool BnAccelerationRelSensor::isCalibrated(){
    if( s_isensor.isCalibrated() ){
        setStatus(SENSOR_STATUS_WORKING);
        return true;
    } else {
        setStatus(SENSOR_STATUS_CALIBRATING);
        return false;
    }
}

BnSensorData BnAccelerationRelSensor::getData(){
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
  sensorData.setValues(s_values, SENSORTYPE_ACCELERATION_REL_TAG);
  return sensorData;
}

BnType BnAccelerationRelSensor::getType(){
    return SENSORTYPE_ACCELERATION_REL_TAG;
}

void BnAccelerationRelSensor::setEnable(bool enable_status){
    s_enabled = enable_status;
}

bool BnAccelerationRelSensor::isEnabled(){
    return s_enabled;
}

void BnAccelerationRelSensor::realignAxis(float values[], float revalues[]){

  revalues[0] = MUL_AXIS_X_ACC * values[OUT_AXIS_X_ACC];
  revalues[1] = MUL_AXIS_Y_ACC * values[OUT_AXIS_Y_ACC];
  revalues[2] = MUL_AXIS_Z_ACC * values[OUT_AXIS_Z_ACC];
}

#endif // __BN_ACCELERATION_ABS_SENSOR_H__
