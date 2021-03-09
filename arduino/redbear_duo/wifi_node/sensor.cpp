/**
* MIT License
* 
* Copyright (c) 2019-2020 Manuel Bottini
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

#include "sensor.h"

bool mSensorInit;

imu::Quaternion mLastQuat;
imu::Quaternion mReadQuat;


StatusSensorLED mStatusSensorLED;
unsigned long mLastReadSensorTime;
unsigned long mSensorReconnectionTime;
//At the beginning of each connection with the sensor it seems it returns some 0s. The first 0s are not of my interest.
volatile bool mFirstZeros;

Adafruit_BNO055 mBNO;

void initStatusSensorHMI(){
  pinMode(STATUS_SENSOR_HMI_LED_P, OUTPUT);
  pinMode(STATUS_SENSOR_HMI_LED_M, OUTPUT);
  digitalWrite(STATUS_SENSOR_HMI_LED_P, LOW);
  digitalWrite(STATUS_SENSOR_HMI_LED_M, LOW);
  mStatusSensorLED.on = false;
  mStatusSensorLED.lastToggle = millis();
}

void setStatusSensorHMI_ON(){
  mSensorInit=true;
  digitalWrite(STATUS_SENSOR_HMI_LED_P, LOW);
  mStatusSensorLED.on = false;
  mStatusSensorLED.lastToggle = millis();
}

void setStatusSensorHMI_OFF(){
  mSensorInit=false;
  DEBUG_PRINTLN("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
  digitalWrite(STATUS_SENSOR_HMI_LED_P, HIGH);
  mStatusSensorLED.on = true;
  mStatusSensorLED.lastToggle = millis();
}

void setStatusSensorHMI_BLINK_SLOW(){
  if(millis()-mStatusSensorLED.lastToggle > 500){
    mStatusSensorLED.lastToggle = millis();
    mStatusSensorLED.on = !mStatusSensorLED.on;
    if(mStatusSensorLED.on){
      digitalWrite(STATUS_SENSOR_HMI_LED_P, HIGH);
    } else {
      digitalWrite(STATUS_SENSOR_HMI_LED_P, LOW);
    }
  }
}

void setStatusSensorHMI_BLINK_FAST(){
  if(millis()-mStatusSensorLED.lastToggle > 100){
    mStatusSensorLED.lastToggle = millis();
    mStatusSensorLED.on = !mStatusSensorLED.on;
    if(mStatusSensorLED.on){
      digitalWrite(STATUS_SENSOR_HMI_LED_P, HIGH);
    } else {
      digitalWrite(STATUS_SENSOR_HMI_LED_P, LOW);
    }
  }
}

void initSensor(){
  mLastQuat = imu::Quaternion(-1,-1,-1,-1);
  mBNO = Adafruit_BNO055(55,BNO055_ADDRESS_B);
  mSensorInit=false;
  mLastReadSensorTime=millis();
  mSensorReconnectionTime=millis();
  /* Initialise the sensor */
  if(mBNO.begin(mBNO.OPERATION_MODE_NDOF_FMC_OFF)) {
     mFirstZeros=true;
     setStatusSensorHMI_ON();
  } else {
     setStatusSensorHMI_OFF();
  }
  mBNO.setExtCrystalUse(true);

}

bool checkSensorInit(){
  if(!mSensorInit){
    if(millis()-mSensorReconnectionTime<5000){
      return false;
    }
    DEBUG_PRINTLN("Sensor not connected");
    mSensorReconnectionTime=millis();
    if(mBNO.begin(mBNO.OPERATION_MODE_NDOF_FMC_OFF)) {
      setStatusSensorHMI_ON();
      mFirstZeros=true;
      return true;
    } else {
      setStatusSensorHMI_OFF();
      return false;
    }
  }
  return true;
}

bool isCalibrationOK(){
  uint8_t sys;
  uint8_t gyro;
  uint8_t accel;
  uint8_t mag;
  mBNO.getCalibration(&sys, &gyro, &accel, &mag);
  if(sys < 2){
    DEBUG_PRINT("Calibration sys = ");
    DEBUG_PRINT_DEC(sys);
    DEBUG_PRINT(" , gyro = ");
    DEBUG_PRINT_DEC(gyro);
    DEBUG_PRINT(" , accel = ");
    DEBUG_PRINT_DEC(accel);
    DEBUG_PRINT(" , mag = ");
    DEBUG_PRINTLN_DEC(mag);
    setStatusSensorHMI_BLINK_SLOW();
    return false;
  } else {
    setStatusSensorHMI_ON();
    return true;
  }
}

bool checkReadFromSensor(){
    if(millis()-mLastReadSensorTime<SENSOR_READ_INTERVAL_MS){
      return false;
    }
    mLastReadSensorTime=millis();
  
    sensors_event_t event;
    mBNO.getEvent(&event);
    imu::Quaternion quat = mBNO.getQuat();
    checkConnectionSensorQuat(quat);
    if(noBigChangeQuat(quat)){
      return false;
    }
    mReadQuat = quat;
    return true;
}


imu::Quaternion getReadQuat(){
  return mReadQuat;
}

//I noticed that just before after a disconnection the getEvent returns 0 0 0 and THEN blocks. So I just check before the next call to getEvent blocks everything
void checkConnectionSensorQuat(imu::Quaternion quat){
  if (quat.w() != 0 || quat.x()!=0 || quat.y()!=0 || quat.z()!=0) { 
    mFirstZeros=false;
  }
  
  if (quat.w() == 0 && quat.x()==0 && quat.y()==0 && quat.z()==0 && !mFirstZeros){
    DEBUG_PRINTLN("Sensor might have gotten disconnected!");
    setStatusSensorHMI_OFF();
  }
}

/*
It also updates the changing last values
*/
bool noBigChangeQuat(imu::Quaternion quat) {
  bool nothingChanged=true;

  //w
  if(quat.w()< mLastQuat.w()-BIG_QUAT_DIFF || mLastQuat.w()+BIG_QUAT_DIFF<quat.w()){
    mLastQuat=quat;
    nothingChanged=false;
  }
  
  //x
  if(quat.x()< mLastQuat.x()-BIG_QUAT_DIFF || mLastQuat.x()+BIG_QUAT_DIFF<quat.x()){
    mLastQuat=quat;
    nothingChanged=false;
  }

  //y
  if(quat.y()< mLastQuat.y()-BIG_QUAT_DIFF || mLastQuat.y()+BIG_QUAT_DIFF<quat.y()){
    mLastQuat=quat;
    nothingChanged=false;
  }

  //z
  if(quat.z()< mLastQuat.z()-BIG_QUAT_DIFF || mLastQuat.z()+BIG_QUAT_DIFF<quat.z()){
    mLastQuat=quat;
    nothingChanged=false;
  }

  return nothingChanged;
}
