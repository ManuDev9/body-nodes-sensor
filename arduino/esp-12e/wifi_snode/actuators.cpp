/**
* MIT License
* 
* Copyright (c) 2021 Manuel Bottini
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

#include "actuators.h"

//Since there is only one actuator new actions will override old ones.
struct Vibration_struct {
  unsigned long startTime_ms;
  unsigned long duration_ms;
  char strength;
} Vibration;

void initActuators(){
  pinMode(HAPTIC_MOTOR_PIN_P, OUTPUT); 
  digitalWrite(HAPTIC_MOTOR_PIN_P, LOW);
  Vibration.startTime_ms = 0;
  Vibration.duration_ms = 0;
}

void setAction(Action action){
  if(action.type == ACTION_HAPTIC_INT){
    DEBUG_PRINT("Haptic triggered with duration = ");
    DEBUG_PRINTLN(action.duration_ms);
    Vibration.startTime_ms = millis();
    Vibration.duration_ms = action.duration_ms;
    Vibration.strength = action.strength;
  }
}

void makeActions(){
  if(millis()-Vibration.startTime_ms < Vibration.duration_ms  ){
    DEBUG_PRINTLN("Doing something");
    digitalWrite(HAPTIC_MOTOR_PIN_P, HIGH);
  } else {
    digitalWrite(HAPTIC_MOTOR_PIN_P, LOW);
  }
}
