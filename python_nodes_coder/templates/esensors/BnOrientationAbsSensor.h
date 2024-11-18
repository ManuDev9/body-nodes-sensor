/**
* MIT License
* 
* Copyright (c) 2021-2024 Manuel Bottini
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

#include "BnNodeSpecific.h"

#ifdef ORIENTATION_ABS_SENSOR

#ifndef __BN_ORIENTATION_ABS_SENSOR_H__
#define __BN_ORIENTATION_ABS_SENSOR_H__


#include "BnDatatypes.h"
#include "BnArduinoUtils.h"
#include "BnISensor.h"

class BnOrientationAbsSensor {
public:
    void init();
    bool checkAllOk();
    bool isCalibrated();
    BnSensorData getData();
    String getType();
    void setEnable(bool enable_status);
    bool isEnabled();

private:
    void realignAxis(float values[], float revalues[]);

    BnISensor s_isensor;
    bool s_enabled;
    bool s_sensorInit;
    unsigned long s_lastReadSensorTime;
    unsigned long s_sensorReconnectionTime;
    //At the beginning of each connection with the sensor it seems it returns some 0s. The first 0s are not of my interest.
    volatile bool s_firstZeros;
    float s_values[4];

};
#endif /*__BN_ORIENTATION_ABS_SENSOR_H__*/

#endif // ORIENTATION_ABS_SENSOR


