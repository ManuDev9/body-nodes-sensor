/**
* MIT License
*
* Copyright (c) 2022 Manuel Bottini
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

#include "basics.h"

#ifdef BODYNODE_GLOVE_SENSOR

#ifndef __WIFI_NODE_GLOVESENSOR_SERIALREADER_H__
#define __WIFI_NODE_GLOVESENSOR_SERIALREADER_H__

class GloveSensorReaderSerial {
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

#endif /*__WIFI_NODE_GLOVESENSOR_SERIALREADER_H__*/

#endif /*BODYNODE_GLOVE_SENSOR*/
