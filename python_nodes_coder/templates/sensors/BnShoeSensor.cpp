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


#include "BnShoeSensor.h"

#ifdef __BN_SHOE_SENSOR_H__

void BnShoeSensor::init() {
    ss_value = 0;
    ss_pin = SHOE_SENSOR_PIN_P;
    pinMode(STATUS_SENSOR_HMI_LED_M, INPUT);
    ss_enabled = true;
}

bool BnShoeSensor::checkAllOk() {
    return true;
}

// 1 value is expected
void BnShoeSensor::getData(int *values){
    values[0] = digitalRead(ss_pin);
    DEBUG_PRINT("Reading from BnShoeSensor value = ");
    DEBUG_PRINTLN(values[0]);
}

String BnShoeSensor::getType(){
    return SENSORTYPE_SHOE_TAG;
}

void BnShoeSensor::setEnable(bool enable_status){
    ss_enabled = enable_status;
}

bool BnShoeSensor::isEnabled(){
    return ss_enabled;
}

#endif /*__BN_SHOE_SENSOR_H__*/