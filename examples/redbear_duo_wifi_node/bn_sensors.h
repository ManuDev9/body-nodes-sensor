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

// The Adafruit libraries are available for all boards
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include "bn_node_specific.h"
#include "bn_datatypes.h"
#include "bn_utils.h"

#ifndef __BN_SENSORS_H__
#define __BN_SENSORS_H__

class BnSensor {
public:
  void init();
  bool checkAllOk();
  bool isCalibrated();
  BnSensorData getData();
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
  BnStatusLED s_statusSensorLED;
  unsigned long s_lastReadSensorTime;
  unsigned long s_sensorReconnectionTime;
  //At the beginning of each connection with the sensor it seems it returns some 0s. The first 0s are not of my interest.
  volatile bool s_firstZeros;
  float s_values[4];

};

#ifdef BODYNODE_GLOVE_SENSOR

class BnGloveSensorReaderSerial {
public:
  // Initializes the reader
  void init();
  // Reads from the serial. Returns true if a full read has been received, false otherwise.
  bool checkAllOk();
  // Returns the data read
  void getData(int *values);
  // Returns the type of the sensor as string
  String getType();
  // Enable/Disable Sensor
  void setEnable(bool enable_status);
  // Returns if sensor is enabled or not
  bool isEnabled();

private:
  boolean grs_lineDone;
  String grs_lineToPrint;
  bool grs_enabled;
};

#endif /*BODYNODE_GLOVE_SENSOR*/

#ifdef BODYNODE_SHOE_SENSOR

class BnShoeSensor {
public:
  // Initializes the reader
  void init();
  // Reads from the serial. Returns true if a full read has been received, false otherwise.
  bool checkAllOk();
  // Returns the data read
  void getData(int *values);
  // Returns the type of the sensor as string
  String getType();
  // Enable/Disable Sensor
  void setEnable(bool enable_status);
  // Returns if sensor is enabled or not
  bool isEnabled();

private:
  int ss_value;
  int ss_pin;
  bool ss_enabled;
};

#endif /*BODYNODE_SHOE_SENSOR*/

#endif /*__BN_SENSORS_H__*/
