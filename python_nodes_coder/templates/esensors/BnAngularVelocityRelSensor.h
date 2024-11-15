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

#ifdef ANGULARVELOCITY_REL_SENSOR

#ifndef __BN_ANGULARVELOCITY_REL_SENSOR_H__
#define __BN_ANGULARVELOCITY_REL_SENSOR_H__


#include "BnDatatypes.h"
#include "BnArduinoUtils.h"
#include "BnISensor.h"

class BnAngularVelocityRelSensor {
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

    BnISensor s_isensor;
    bool s_enabled;
    bool s_sensorInit;
    unsigned long s_lastReadSensorTime;
    unsigned long s_sensorReconnectionTime;
    float s_values[3];

};
#endif /*__BN_ANGULARVELOCITY_REL_SENSOR_H__*/

#endif // ANGULARVELOCITY_REL_SENSOR


