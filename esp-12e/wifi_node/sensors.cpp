/**
* MIT License
*
* Copyright (c) 2021-2022 Manuel Bottini
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

#include "sensors.h"

void Sensor::init(){
  s_enabled = true;
  pinMode(STATUS_SENSOR_HMI_LED_P, OUTPUT);
  analogWrite(STATUS_SENSOR_HMI_LED_P, 0);
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

void Sensor::setStatus(int sensor_status){
  if(sensor_status == SENSOR_STATUS_NOT_ACCESSIBLE){
    s_sensorInit=false;
    DEBUG_PRINTLN("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    analogWrite(STATUS_SENSOR_HMI_LED_P, LED_DT_ON);
    s_statusSensorLED.on = true;
    s_statusSensorLED.lastToggle = millis();
  } else if(sensor_status == SENSOR_STATUS_CALIBRATING) {
    if(millis()-s_statusSensorLED.lastToggle > 500){
      s_statusSensorLED.lastToggle = millis();
      s_statusSensorLED.on = !s_statusSensorLED.on;
      if(s_statusSensorLED.on){
        analogWrite(STATUS_SENSOR_HMI_LED_P, LED_DT_ON);
      } else {
        analogWrite(STATUS_SENSOR_HMI_LED_P, 0);
      }
    }
  } else if(sensor_status == SENSOR_STATUS_WORKING) {
    s_sensorInit=true;
    analogWrite(STATUS_SENSOR_HMI_LED_P, 0);
    s_statusSensorLED.on = false;
    s_statusSensorLED.lastToggle = millis();
  }
}

bool Sensor::checkAllOk(){
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
  float svalues[4] = { sensor_quat.w(), sensor_quat.x(), sensor_quat.y(), sensor_quat.z() };
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

bool Sensor::isCalibrated(){
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

void Sensor::getData(float *values){
  s_lastReadSensorTime=millis();

  values[0] = s_values[0];
  values[1] = s_values[1];
  values[2] = s_values[2];
  values[3] = s_values[3];

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
}

String Sensor::getType(){
  return SENSOR_DATA_TYPE_ORIENTATION_ABS_TAG;
}

void Sensor::setEnable(bool enable_status){
  s_enabled = enable_status;
}

bool Sensor::isEnabled(){
  return s_enabled;
}

void Sensor::realignAxis(float values[], float revalues[]){

  // Axis W
  #if OUT_AXIS_W == SENSOR_AXIS_W
  revalues[0] = values[0];
  #elif OUT_AXIS_W == SENSOR_AXIS_X
  revalues[0] = values[1];
  #elif OUT_AXIS_W == SENSOR_AXIS_Y
  revalues[0] = values[2];
  #elif OUT_AXIS_W == SENSOR_AXIS_Z
  revalues[0] = values[3];
  #endif

  // Axis X
  #if OUT_AXIS_X == SENSOR_AXIS_W
  revalues[1] = values[0];
  #elif OUT_AXIS_X == SENSOR_AXIS_X
  revalues[1] = values[1];
  #elif OUT_AXIS_X == SENSOR_AXIS_Y
  revalues[1] = values[2];
  #elif OUT_AXIS_X == SENSOR_AXIS_Z
  revalues[1] = values[3];
  #endif

  // Axis Y
  #if OUT_AXIS_Y == SENSOR_AXIS_W
  revalues[2] = values[0];
  #elif OUT_AXIS_Y == SENSOR_AXIS_X
  revalues[2] = values[1];
  #elif OUT_AXIS_Y == SENSOR_AXIS_Y
  revalues[2] = values[2];
  #elif OUT_AXIS_Y == SENSOR_AXIS_Z
  revalues[2] = values[3];
  #endif

  // Axis Z
  #if OUT_AXIS_Z == SENSOR_AXIS_W
  revalues[3] = values[0];
  #elif OUT_AXIS_Z == SENSOR_AXIS_X
  revalues[3] = values[1];
  #elif OUT_AXIS_Z == SENSOR_AXIS_Y
  revalues[3] = values[2];
  #elif OUT_AXIS_Z == SENSOR_AXIS_Z
  revalues[3] = values[3];
  #endif

  revalues[0] = MUL_AXIS_W * revalues[0];
  revalues[1] = MUL_AXIS_X * revalues[1];
  revalues[2] = MUL_AXIS_Y * revalues[2];
  revalues[3] = MUL_AXIS_Z * revalues[3];
}

#ifdef BODYNODE_GLOVE_SENSOR

void GloveSensorReaderSerial::init() {
  Serial.begin(115200);
  while (!Serial) {
    delay (1000);
  }
  grs_lineDone = false;
  grs_lineToPrint = "";
  grs_enabled = true;
}


bool GloveSensorReaderSerial::checkAllOk() {
  while(Serial.peek() != -1 && grs_lineDone == false) {
    uint8_t byteVal = Serial.read();

    // In ASCII encoding, \n is the Newline character 0x0A (decimal 10), \r is the Carriage Return character 0x0D (decimal 13).
    if(byteVal == 13 || byteVal == 10) {
      grs_lineDone = true;
    } else {
      grs_lineToPrint += String((char)(byteVal));
    }
    Serial.flush();
  }
  return grs_lineDone;
}

// 9 values are expected
void GloveSensorReaderSerial::getData(int *values){
  if(grs_lineDone == false){
    return;
  }
  const int str_len = grs_lineToPrint.length() + 1;
  char buf[str_len];
  grs_lineToPrint.toCharArray(buf, str_len);
  grs_lineToPrint = "";
  grs_lineDone = false;

  char *p_end = &buf[0];
  for(uint8_t counter = 0; counter < 9; ++counter) {
    values[counter] = (int)(strtol(p_end, &p_end, 10));
  }
}

String GloveSensorReaderSerial::getType(){
  return SENSOR_DATA_TYPE_GLOVE_TAG;
}

void GloveSensorReaderSerial::setEnable(bool enable_status){
  grs_enabled = enable_status;
}

bool GloveSensorReaderSerial::isEnabled(){
  return grs_enabled;
}

#endif /*BODYNODE_GLOVE_SENSOR*/
