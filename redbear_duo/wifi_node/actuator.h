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

#include "node_specific.h"
#include "commons.h"

#ifndef __WIFI_NODE_ACTUATOR_H__
#define __WIFI_NODE_ACTUATOR_H__

//Since there is only one actuator new actions will override old ones.
struct Vibration_struct {
  unsigned long startTime_ms;
  unsigned long duration_ms;
  uint16_t strength;
};

class Actuator {
public:
  void init();
  void setAction(JsonObject &action);
  void performAction();
  String getType();

private:
  Vibration_struct a_vibration;
};

#endif //__WIFI_NODE_ACTUATOR_H__
