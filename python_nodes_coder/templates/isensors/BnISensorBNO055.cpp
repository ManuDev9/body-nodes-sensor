/**
* MIT License
*
* Copyright (c) 2024-2026 Manuel Bottini
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


#include "Adafruit_Sensor.h"
#include "Adafruit_BNO055.h"

#if defined(BN_BNO055_I2C1_SDA1) && defined(BN_BNO055_I2C1_SCL1)
#include <Wire.h>
TwoWire sWire(0);
#endif

static Adafruit_BNO055 s_BNO;
static bool sIsInit = false;
static BnStatusLED sStatusSensorLED;

bool BnISensor::init(){
    if(sIsInit){
        return true;
    }

#if defined(BN_BNO055_I2C1_SDA1) && defined(BN_BNO055_I2C1_SCL1)
    s_BNO = Adafruit_BNO055(55, BN_BNO055_ADDRESS, &sWire );
    //sWire.end(); // release the bus first. Removed for now because not all Wire objects have end()
    sWire.begin(BN_BNO055_I2C1_SDA1, BN_BNO055_I2C1_SCL1);
    sWire.setClock(100000);   // slow down for clock stretching
    sWire.setTimeOut(100);    // ms, avoids bus lockups
#else
    s_BNO = Adafruit_BNO055(55, BN_BNO055_ADDRESS);
    //Wire.end(); // release the bus first. Removed for now because not all Wire objects have end()
    Wire.setClock(100000);   // slow down for clock stretching
    Wire.setTimeout(100);    // ms, avoids bus lockups
#endif

    delay(800); // let the BNO055 finish its power-on reset

    /* Initialise the sensor */
    if(s_BNO.begin(OPERATION_MODE_NDOF_FMC_OFF)) {
        delay(500); // give fusion time to produce output
        setStatus(BN_SENSOR_STATUS_WORKING);
    } else {
        setStatus(BN_SENSOR_STATUS_NOT_ACCESSIBLE);
    }
    s_BNO.setExtCrystalUse(BN_BNO055_EXTERNALCRYSTAL);
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
        setStatus(BN_SENSOR_STATUS_CALIBRATING);
        return false;
    } else {
        setStatus(BN_SENSOR_STATUS_WORKING);
        return true;
    }
}

bool bnoProbe() {
  uint8_t id = 0, mode = 0, stat = 0;

#if defined(BN_BNO055_I2C1_SDA1) && defined(BN_BNO055_I2C1_SCL1)

  sWire.beginTransmission(BN_BNO055_ADDRESS);
  sWire.write(0x00);                       // CHIP_ID register
  uint8_t err = sWire.endTransmission(true);
  int n = sWire.requestFrom((int)BN_BNO055_ADDRESS, 1);
  if (n == 1) id = sWire.read();

  sWire.beginTransmission(BN_BNO055_ADDRESS);
  sWire.write(0x3D);                       // OPR_MODE register
  sWire.endTransmission(true);
  if (sWire.requestFrom((int)BN_BNO055_ADDRESS, 1) == 1) mode = sWire.read();

  sWire.beginTransmission(BN_BNO055_ADDRESS);
  sWire.write(0x39);                       // SYS_STATUS register
  sWire.endTransmission(true);
  if (sWire.requestFrom((int)BN_BNO055_ADDRESS, 1) == 1) stat = sWire.read();

#else

  Wire.beginTransmission(BN_BNO055_ADDRESS);
  Wire.write(0x00);                       // CHIP_ID register
  uint8_t err = Wire.endTransmission(true);
  int n = Wire.requestFrom((int)BN_BNO055_ADDRESS, 1);
  if (n == 1) id = Wire.read();

  Wire.beginTransmission(BN_BNO055_ADDRESS);
  Wire.write(0x3D);                       // OPR_MODE register
  Wire.endTransmission(true);
  if (Wire.requestFrom((int)BN_BNO055_ADDRESS, 1) == 1) mode = Wire.read();

  Wire.beginTransmission(BN_BNO055_ADDRESS);
  Wire.write(0x39);                       // SYS_STATUS register
  Wire.endTransmission(true);
  if (Wire.requestFrom((int)BN_BNO055_ADDRESS, 1) == 1) stat = Wire.read();

#endif

  DEBUG_PRINTF("bnoProbe txErr=%u bytes=%d id=0x%02X mode=%u sysStatus=%u\n",
                err, n, id, mode, stat);

  return id == 0xA0;
}

bool BnISensor::getData(float values[], const int type){

    if( type == BN_ISENSOR_DATATYPE_ACCELEROMETER ){
        imu::Vector<3> vector_acc = s_BNO.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
        values[0] = static_cast<float>( vector_acc[0] );
        values[1] = static_cast<float>( vector_acc[1] );
        values[2] = static_cast<float>( vector_acc[2] );
        return true;
    } else if( type == BN_ISENSOR_DATATYPE_GYROSCOPE ){
        imu::Vector<3> vector_gyro = s_BNO.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
        values[0] =  static_cast<float>( vector_gyro[0] );
        values[1] =  static_cast<float>( vector_gyro[1] );
        values[2] =  static_cast<float>( vector_gyro[2] );
        return true;
    } else if( type == BN_ISENSOR_DATATYPE_MAGNETOMETER ){
        imu::Vector<3> vector_magn = s_BNO.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
        values[0] =  static_cast<float>( vector_magn[0] );
        values[1] =  static_cast<float>( vector_magn[1] );
        values[2] =  static_cast<float>( vector_magn[2] );
        return true;
    } else if( type == BN_ISENSOR_DATATYPE_ABSOLUTEORIENTATION ){
        imu::Quaternion sensor_quat = s_BNO.getQuat();
        values[0] =  static_cast<float>( sensor_quat.w() );
        values[1] =  static_cast<float>( sensor_quat.x() );
        values[2] =  static_cast<float>( sensor_quat.y() );
        values[3] =  static_cast<float>( sensor_quat.z() );

        if(values[0] == 0 && values[1] == 0 && values[2] == 0 && values[3] == 0 ) {
            bnoProbe();
        } 
        /*
        DEBUG_PRINT("values = ");
        DEBUG_PRINT(values[0]);
        DEBUG_PRINT(", ");
        DEBUG_PRINT(values[1]);
        DEBUG_PRINT(", ");
        DEBUG_PRINT(values[2]);
        DEBUG_PRINT(", ");
        DEBUG_PRINTLN(values[3]);
        */

        return true;
    } else {
        return false;
    }

}

void BnISensor::setStatus(int BN_SENSOR_status){
    if(BN_SENSOR_status == BN_SENSOR_STATUS_NOT_ACCESSIBLE){
        sIsInit=false;
        DEBUG_PRINTLN("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        BN_NODE_SPECIFIC_BN_ISENSOR_HMI_LED_ON;
        sStatusSensorLED.on = true;
        sStatusSensorLED.lastToggle = millis();
    } else if(BN_SENSOR_status == BN_SENSOR_STATUS_CALIBRATING) {
        if(millis()-sStatusSensorLED.lastToggle > 500){
            sStatusSensorLED.lastToggle = millis();
            sStatusSensorLED.on = !sStatusSensorLED.on;
            if(sStatusSensorLED.on){
                BN_NODE_SPECIFIC_BN_ISENSOR_HMI_LED_ON;
            } else {
                BN_NODE_SPECIFIC_BN_ISENSOR_HMI_LED_OFF;
            }
        }
    } else if(BN_SENSOR_status == BN_SENSOR_STATUS_WORKING) {
        sIsInit=true;
        BN_NODE_SPECIFIC_BN_ISENSOR_HMI_LED_OFF;
        sStatusSensorLED.on = false;
        sStatusSensorLED.lastToggle = millis();
    }
}

