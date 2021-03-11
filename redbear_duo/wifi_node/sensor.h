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

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "basics.h" 

#ifndef __WIFI_NODE_SENSOR_H__
#define __WIFI_NODE_SENSOR_H__

#define SENSOR_READ_INTERVAL_MS 30

#define BIG_ANGLE_DIFF 2
#define BIG_QUAT_DIFF 0.002

struct StatusSensorLED {
  bool on;
  unsigned long lastToggle;
};

bool noBigChangeQuat(imu::Quaternion quat);
void checkConnectionSensorQuat(imu::Quaternion quat);
void sendOrientationValueQuat(imu::Quaternion quat); 

void initStatusSensorHMI();
void setStatusSensorHMI_ON();
void setStatusSensorHMI_OFF();
void setStatusSensorHMI_BLINK_SLOW();
void setStatusSensorHMI_BLINK_FAST();
void initSensor();
bool checkSensorInit();
bool isFiltered();
bool isCalibrationOK();
bool checkReadFromSensor();
imu::Quaternion getReadQuat();


#endif /*__WIFI_NODE_SENSOR_H__*/
