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

#include "basics.h"
#include <utility/imumaths.h>

#ifndef __WIFI_NODE_WIFICONN_H__
#define __WIFI_NODE_WIFICONN_H__

#define WIFI_NOT_CONNECTED 0
#define WIFI_SERVER_WAITING_ACK 1
#define WIFI_SERVER_CONNECTED_NO 2
#define WIFI_SERVER_CONNECTED_OK 3

#define MAX_BUFF_LENGTH 100

void printWifiStatus();
bool tryConnectWifi();
bool tryConnectServer();

void initStatusConnectionHMI();
void setStatusConnectionHMI_ON();
void setStatusConnectionHMI_OFF();
void setStatusConnectionHMI_BLINK();
void setWifiNotConnected();
void setWifiConnected();
void setServerNotConnected();
void setServerConnected();
void setWaitingACK();
bool isWifiConnected();
bool isServerConnected();
bool isWaitingACK();
void checkWifiAndServer();
void initWifi();
bool tryConnectWifi();
void insertInOrientationStringQuat(char buf[], int startPos, int prec,double quatVal);
void sendOrientationValueQuat(imu::Quaternion quat);
void printWifiStatus();
void tryContactServer();
bool checkForACK();
Action checkActionWifi();

#endif //__WIFI_NODE_WIFICONN_H__
