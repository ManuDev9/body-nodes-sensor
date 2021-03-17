/**
* MIT License
* 
* Copyright (c) 2019-2021 Manuel Bottini
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
#include "sensor.h"
#include "wifi_conn.h"
#include "actuators.h"

//SYSTEM_THREAD(ENABLED);
//BLE_SETUP(ENABLED);
#if defined(ARDUINO) 
  SYSTEM_MODE(MANUAL);
#endif

void initStatusHMI(){
  initStatusSensorHMI();
  initStatusConnectionHMI();
}

void setup() {  
  Serial.begin(57600);
  setWifiNotConnected();

  initActuators();
  initStatusHMI();
  initSensor();
  initWifi();
}

void loop() {
  if(!checkSensorInit()){
    return;
  }  
  if(!isCalibrationOK()){
  }
  checkWifiAndServer();
  if(isServerConnected()){
    if(checkReadFromSensor()){
      sendOrientationValueQuat(getReadQuat());
    }
    Action action = checkActionWifi();
    if(action.message.length() > 0){
      setAction(action);
      sendACK();
    }
  }
  makeActions();
}
