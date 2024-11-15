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

#include "Adafruit_Sensor.h"
#include "Adafruit_BNO055.h"

static Adafruit_BNO055 s_BNO;
static bool sIsInit = false;

bool BnISensor::init(){
    if(sIsInit){
        return true;
    }

    s_BNO = Adafruit_BNO055(55, BNO055_ADDRESS_B);
    /* Initialise the sensor */
    if(s_BNO.begin(s_BNO.OPERATION_MODE_NDOF_FMC_OFF)) {
        sIsInit = true;
    } else {
        sIsInit = false;
    }
    s_BNO.setExtCrystalUse(true);
    return sIsInit;
}

bool BnISensor::isCalibrated(){
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
        return false;
    } else {
        return true;
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
        imu::Vector<3> vector_acc = s_BNO.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
        values[0] = static_cast<float>( vector_acc[0] );
        values[1] = static_cast<float>( vector_acc[1] );
        values[2] = static_cast<float>( vector_acc[2] );
        return true;
    } else if( type == ISENSOR_DATATYPE_GYROSCOPE ){
        imu::Vector<3> vector_gyro = s_BNO.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
        values[0] =  static_cast<float>( vector_gyro[0] );
        values[1] =  static_cast<float>( vector_gyro[1] );
        values[2] =  static_cast<float>( vector_gyro[2] );
        return true;
    } else if( type == ISENSOR_DATATYPE_MAGNETOMETER ){
        imu::Vector<3> vector_magn = s_BNO.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
        values[0] =  static_cast<float>( vector_magn[0] );
        values[1] =  static_cast<float>( vector_magn[1] );
        values[2] =  static_cast<float>( vector_magn[2] );
        return true;
    } else if( type == ISENSOR_DATATYPE_ABSOLUTEORIENTATION ){
        imu::Quaternion sensor_quat = s_BNO.getQuat();
        values[0] =  static_cast<float>( sensor_quat.w() );
        values[1] =  static_cast<float>( sensor_quat.x() );
        values[2] =  static_cast<float>( sensor_quat.y() );
        values[3] =  static_cast<float>( sensor_quat.z() );
        return true;
    } else {
        return false;
    }

}

#endif /*__BN_ISENSOR_H__*/
