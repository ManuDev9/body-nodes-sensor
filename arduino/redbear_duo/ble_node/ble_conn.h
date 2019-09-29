/**
* MIT License
* 
* Copyright (c) 2019 Manuel Bottini
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
#include <utility/imumaths.h>

#ifndef __BLE_NODE_CONN_H__
#define __BLE_NODE_CONN_H__

//#define BLE_MIN_INTERVAL          0x0028 // 50ms.
//#define BLE_MAX_INTERVAL          0x0190 // 500ms.

#define BLE_MIN_INTERVAL    0x0006 // 7.5ms (7.5 / 1.25)
#define BLE_MAX_INTERVAL    0x0018 // 30ms (30 / 1.25)
#define SLAVE_LATENCY              0x0000 // No slave latency.
#define CONN_SUPERVISION_TIMEOUT   0x03E8 // 10s.

#define BLE_PERIPHERAL_APPEARANCE  BLE_APPEARANCE_UNKNOWN

#define BYTES_FLOAT_QUAT 4
#define DIMENSIONS_OF_QUAT 4

#define CHARACTERISTIC_ORI_MAX_LEN (BYTES_FLOAT_QUAT*DIMENSIONS_OF_QUAT)
#define CHARACTERISTIC_ACT_MAX_LEN 20

typedef union
{
 float number;
 unsigned char bytes[4];
} float_converter;


void initStatusConnectionHMI();
void setStatusConnectionHMI_ON();
void setStatusConnectionHMI_OFF();
void sendOrientationValueQuat(imu::Quaternion quat); 
void initBLE();
bool isConnOK();
Action checkActionBLE();

#endif //__BLE_NODE_CONN_H__
