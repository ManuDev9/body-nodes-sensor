/**
* MIT License
* 
* Copyright (c) 2021 Manuel Bottini
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

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "basics.h" 
#include "commons.h"

#ifndef __WIFI_NODE_SENSOR_H__
#define __WIFI_NODE_SENSOR_H__

class Sensor {
public:
  void init();
  bool checkAllOk();
  bool isCalibrated();
  void getData(float *values);
  String getType();
  void setEnable(bool enable_status);
  bool isEnabled();

private:
  void setStatus(int sensor_status);
  void realignAxis(float values[], float revalues[]);

  bool s_enabled;
  Adafruit_BNO055 s_BNO;
  bool s_sensorInit;
  imu::Quaternion s_lastQuat;
  StatusLED s_statusSensorLED;
  unsigned long s_lastReadSensorTime;
  unsigned long s_sensorReconnectionTime;
  //At the beginning of each connection with the sensor it seems it returns some 0s. The first 0s are not of my interest.
  volatile bool s_firstZeros;

};

#endif /*__WIFI_NODE_SENSOR_H__*/
