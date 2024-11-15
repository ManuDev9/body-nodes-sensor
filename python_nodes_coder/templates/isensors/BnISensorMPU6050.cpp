/**
* MIT License
* 
* Copyright (c) 2024 Manuel Bottini
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

// You need to install the Adafruit_MPU6050 package from Tools->Manage Libraries...
#include "Adafruit_MPU6050.h"

// Note: You need to set the appropriate SDA and SCL pins on the BnNodeSpecific.h file (or anywhere is your project, but there is better)
static TwoWire sMPU6050Wire(NRF_TWIM0, NRF_TWIS0, SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn, MPU6050_PIN_SDA, MPU6050_PIN_SCL);
static Adafruit_MPU6050 sMPU;
static bool sIsInit = false;

bool BnISensor::init(){
    if(sIsInit){
        return true;
    }

    /* Initialise the sensor */
    sMPU6050Wire.begin();
    if(sMPU.begin(MPU6050_I2CADDR_DEFAULT, &sMPU6050Wire) ){
        sIsInit = true;
    } else {
        sIsInit = false;
    }
    return sIsInit;
}

bool BnISensor::isCalibrated(){
    return true;
}

bool BnISensor::getData(float values[], const int type){

    sensors_event_t a, g;
    sMPU.getAccelerometerSensor()->getEvent(&a);    // outputs m/s^2
    sMPU.getGyroSensor()->getEvent(&g);             // outputs in rad/s

    if( type == ISENSOR_DATATYPE_ACCELEROMETER ){
        values[0] =  a.acceleration.x;
        values[1] =  a.acceleration.y;
        values[2] =  a.acceleration.z;
        return true;
    } else if( type == ISENSOR_DATATYPE_GYROSCOPE ){
        values[0] =  g.gyro.x;
        values[1] =  g.gyro.y;
        values[2] =  g.gyro.z;
        return true;        
    } else if( type == ISENSOR_DATATYPE_MAGNETOMETER ){
        return false;
    } else if( type == ISENSOR_DATATYPE_ABSOLUTEORIENTATION ){
        return false;        
    } else {
        return false;
    }

}

#endif /*__BN_ISENSOR_H__*/