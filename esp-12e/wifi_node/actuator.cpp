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

#include "actuator.h"

void Actuator::init(){
  pinMode(HAPTIC_MOTOR_PIN_P, OUTPUT); 
  digitalWrite(HAPTIC_MOTOR_PIN_P, LOW);
  a_vibration.startTime_ms = 0;
  a_vibration.duration_ms = 0;
}

void Actuator::setAction(JsonObject &action){
  if(action["type"] == ACTION_TYPE_HAPTIC_TAG){
    DEBUG_PRINT("Haptic triggered with duration and strenght = ");
    uint16_t duration_ms = action["duration_ms"];
    uint16_t strength = action["strength"];
    DEBUG_PRINTLN(duration_ms);
    DEBUG_PRINTLN(strength);
    a_vibration.startTime_ms = millis();
    a_vibration.duration_ms = duration_ms;
  }
}

void Actuator::performAction(){
  if(millis()-a_vibration.startTime_ms < a_vibration.duration_ms  ){
    DEBUG_PRINTLN("Doing something");
    digitalWrite(HAPTIC_MOTOR_PIN_P, HIGH);
  } else {
    digitalWrite(HAPTIC_MOTOR_PIN_P, LOW);
  }
}

String Actuator::getType(){
  // It is well known for this Bodynode
  return ACTION_TYPE_HAPTIC_TAG;
}
