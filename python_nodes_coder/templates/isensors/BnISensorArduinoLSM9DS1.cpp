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

#include "BnISensor.h"

#ifdef __BN_ISENSOR_H__

// You need to install the Arduino_LSM9DS1 package from Tools->Manage Libraries...
#include "Arduino_LSM9DS1.h"

static bool sIsInit = false;
static BnStatusLED sStatusSensorLED;

bool BnISensor::init(){

    if(sIsInit){
        return true;
    }

    /* Initialise the sensor */
    if( IMU.begin() ){
         setStatus(SENSOR_STATUS_WORKING);
    } else {
         setStatus(SENSOR_STATUS_NOT_ACCESSIBLE);
    }
    return sIsInit;
}

bool BnISensor::isCalibrated(){
    if( IMU.gyroscopeAvailable() && IMU.accelerationAvailable() && IMU.magneticFieldAvailable() ){
        setStatus(SENSOR_STATUS_WORKING);
        return true;
    } else {
        setStatus(SENSOR_STATUS_CALIBRATING);
        return false;
    }
}

bool BnISensor::getData(float values[], const int type){
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
    if( type == ISENSOR_DATATYPE_ACCELEROMETER ){
        float accx;
        float accy;
        float accz;
        IMU.readAcceleration(accx, accy, accz); // outputs in g, convert to m/s^2
        values[0] = accx;
        values[1] = accy;
        values[2] = accz;
        return true;
    } else if( type == ISENSOR_DATATYPE_GYROSCOPE ){
        float gyrox;
        float gyroy;
        float gyroz;
        IMU.readGyroscope(gyrox, gyroy, gyroz);  // outputs in dps, covert it to rad/s
        values[0] = gyrox;
        values[1] = gyroy;
        values[2] = gyroz;
        return true;        
    } else if( type == ISENSOR_DATATYPE_MAGNETOMETER ){
        float magnx;
        float magny;
        float magnz;
        IMU.readMagneticField(magnx, magny, magnz);  // outputs in uT
        values[0] = magnx;
        values[1] = magny;
        values[2] = magnz;
        return true;
    } else if( type == ISENSOR_DATATYPE_ABSOLUTEORIENTATION ){
        return false;        
    } else {
        return false;
    }

}

void BnISensor::setStatus(int sensor_status){
    if(sensor_status == SENSOR_STATUS_NOT_ACCESSIBLE){
        sIsInit=false;
        DEBUG_PRINTLN("Ooops, no LSM9DS1 detected ... Check your wiring or I2C ADDR!");
        BN_NODE_SPECIFIC_BN_ISENSOR_HMI_LED_ON;
        sStatusSensorLED.on = true;
        sStatusSensorLED.lastToggle = millis();
    } else if(sensor_status == SENSOR_STATUS_CALIBRATING) {
        if(millis()-sStatusSensorLED.lastToggle > 500){
            sStatusSensorLED.lastToggle = millis();
            sStatusSensorLED.on = !sStatusSensorLED.on;
            if(sStatusSensorLED.on){
                BN_NODE_SPECIFIC_BN_ISENSOR_HMI_LED_ON;
            } else {
                BN_NODE_SPECIFIC_BN_ISENSOR_HMI_LED_OFF;
            }
        }
    } else if(sensor_status == SENSOR_STATUS_WORKING) {
        sIsInit=true;
        BN_NODE_SPECIFIC_BN_ISENSOR_HMI_LED_OFF;
        sStatusSensorLED.on = false;
        sStatusSensorLED.lastToggle = millis();
    }
}

#endif // __BN_ISENSOR_H__
