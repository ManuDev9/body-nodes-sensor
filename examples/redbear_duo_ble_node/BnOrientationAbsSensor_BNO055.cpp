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

#include "BnOrientationAbsSensor_BNO055.h"

#ifdef __BN_ORIENTATION_ABS_SENSOR_BNO055_H__

void BnOrientationAbsSensor::init(){
    s_enabled = true;
    BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_HMI_LED_SETUP;
    BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_HMI_LED_ON;
    s_statusSensorLED.on = false;
    s_statusSensorLED.lastToggle = millis();

    s_lastQuat = imu::Quaternion(-1,-1,-1,-1);
    s_BNO = Adafruit_BNO055(55, BNO055_ADDRESS_B);
    s_sensorInit=false;
    s_lastReadSensorTime=millis();
    s_sensorReconnectionTime=millis();
    /* Initialise the sensor */
    if(s_BNO.begin(s_BNO.OPERATION_MODE_NDOF_FMC_OFF)) {
         s_firstZeros=true;
         setStatus(SENSOR_STATUS_WORKING);
    } else {
         setStatus(SENSOR_STATUS_NOT_ACCESSIBLE);
    }
    s_BNO.setExtCrystalUse(true);
}

void BnOrientationAbsSensor::setStatus(int sensor_status){
    if(sensor_status == SENSOR_STATUS_NOT_ACCESSIBLE){
        s_sensorInit=false;
        DEBUG_PRINTLN("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_HMI_LED_ON;
        s_statusSensorLED.on = true;
        s_statusSensorLED.lastToggle = millis();
    } else if(sensor_status == SENSOR_STATUS_CALIBRATING) {
        if(millis()-s_statusSensorLED.lastToggle > 500){
            s_statusSensorLED.lastToggle = millis();
            s_statusSensorLED.on = !s_statusSensorLED.on;
            if(s_statusSensorLED.on){
                BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_HMI_LED_ON;
            } else {
                BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_HMI_LED_OFF;
            }
        }
    } else if(sensor_status == SENSOR_STATUS_WORKING) {
        s_sensorInit=true;
        BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_HMI_LED_OFF;
        s_statusSensorLED.on = false;
        s_statusSensorLED.lastToggle = millis();
    }
}

bool BnOrientationAbsSensor::checkAllOk(){
    if(!s_sensorInit){
        if(millis()-s_sensorReconnectionTime<5000){
            return false;
        }
        DEBUG_PRINTLN("Sensor not connected");
        s_sensorReconnectionTime=millis();
        if(s_BNO.begin(s_BNO.OPERATION_MODE_NDOF_FMC_OFF)) {
            setStatus(SENSOR_STATUS_WORKING);
            s_firstZeros=true;
            return true;
        } else {
            setStatus(SENSOR_STATUS_NOT_ACCESSIBLE);
            return false;
        }
    }
    if(millis()-s_lastReadSensorTime<SENSOR_READ_INTERVAL_MS){
        return false;
    }

    sensors_event_t event;
    s_BNO.getEvent(&event);
    imu::Quaternion sensor_quat = s_BNO.getQuat();
    float svalues[4] = { 
        static_cast<float>( sensor_quat.w() ),
        static_cast<float>( sensor_quat.x() ),
        static_cast<float>( sensor_quat.y() ),
        static_cast<float>( sensor_quat.z() )
    };
    float tvalues[4];
    realignAxis(svalues, tvalues);

    //I noticed that just before after a disconnection the getEvent returns 0 0 0 and THEN blocks. So I just check before the next call to getEvent blocks everything
    if (tvalues[0] != 0 || tvalues[1]!=0 || tvalues[2]!=0 || tvalues[3]!=0) {
        s_firstZeros=false;
    }

    if (tvalues[0] == 0 && tvalues[1]==0 && tvalues[2]==0 && tvalues[3]==0 && !s_firstZeros){
        DEBUG_PRINTLN("Sensor might have gotten disconnected!");
        setStatus(SENSOR_STATUS_NOT_ACCESSIBLE);
        return false;
    }

    s_values[0] = tvalues[0];
    s_values[1] = tvalues[1];
    s_values[2] = tvalues[2];
    s_values[3] = tvalues[3];

    return true;
}

bool BnOrientationAbsSensor::isCalibrated(){
    uint8_t sys;
    uint8_t gyro;
    uint8_t accel;
    uint8_t mag;
    s_BNO.getCalibration(&sys, &gyro, &accel, &mag);
    if(sys < 2){
        /*
        DEBUG_PRINT("Calibration sys = ");
        DEBUG_PRINT_DEC(sys);
        DEBUG_PRINT(" , gyro = ");
        DEBUG_PRINT_DEC(gyro);
        DEBUG_PRINT(" , accel = ");
        DEBUG_PRINT_DEC(accel);
        DEBUG_PRINT(" , mag = ");
        DEBUG_PRINTLN_DEC(mag);
        */
        setStatus(SENSOR_STATUS_CALIBRATING);
        return false;
    } else {
        setStatus(SENSOR_STATUS_WORKING);
        return true;
    }
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

    revalues[0] = MUL_AXIS_W * values[OUT_AXIS_W];
    revalues[1] = MUL_AXIS_X * values[OUT_AXIS_X];
    revalues[2] = MUL_AXIS_Y * values[OUT_AXIS_Y];
    revalues[3] = MUL_AXIS_Z * values[OUT_AXIS_Z];
}

#endif /*__BN_ORIENTATION_ABS_SENSOR_BNO055_H__*/
