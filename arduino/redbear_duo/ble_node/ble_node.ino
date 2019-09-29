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

//THIS IS COMPATIBLE ONLY FOR REDBEAR DUO, BLE MODE

#include "basics.h"
#include "sensor.h"
#include "ble_conn.h"
#include "actuators.h"

#if defined(ARDUINO) 
  SYSTEM_MODE(MANUAL);
#endif


void initStatusHMI(){
  initStatusSensorHMI();
  initStatusConnectionHMI();
}

///MAIN PROGRAM
void setup() {
  Serial.begin(57600);

  initActuators();
  initStatusHMI();
  initSensor();
  initBLE();  
  setStatusSensorHMI_ON();
}


void loop() {
  if(!checkSensorInit()){
    return;
  }
  if(!isCalibrationOK()){
  }

  if(isConnOK()){
    if(checkReadFromSensor()){
      sendOrientationValueQuat(getReadQuat());
    }
    Action action = checkActionBLE();
    setAction(action);
  }   
  makeActions();  
}
