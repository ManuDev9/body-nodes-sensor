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

#include "bn_constants.h"
#include "bn_datatypes.h"
#include "bn_node_specific.h"

#ifndef __BN_UTILS_H
#define __BN_UTILS_H

class BnPersMemory {
public:
  static void init();
  static void clean();
  static void setValue(BnKey key, String value);
  static String getValue(BnKey key);
private:
  BnPersMemory(){};

  static constexpr uint8_t pm_checkkey[5] = {0x00, 0x00, 0x00, 0x00, 0x01};

  static constexpr uint16_t pm_player_addr_nbytes = 50;
  static constexpr uint16_t pm_player_addr_chars = 51;
  
  static constexpr uint16_t pm_bodypart_addr_nbytes = 100;
  static constexpr uint16_t pm_bodypart_addr_chars = 101;

  static constexpr uint16_t pm_bodypart_glove_addr_nbytes = 150;
  static constexpr uint16_t pm_bodypart_glove_addr_chars = 151;

  static constexpr uint16_t pm_bodypart_shoe_addr_nbytes = 200;
  static constexpr uint16_t pm_bodypart_shoe_addr_chars = 201;

  static constexpr uint16_t pm_ssid_addr_nbytes = 250;
  static constexpr uint16_t pm_ssid_addr_chars = 251;

  static constexpr uint16_t pm_password_addr_nbytes = 300;
  static constexpr uint16_t pm_password_addr_chars = 301;

  static constexpr uint16_t pm_multicast_message_addr_nbytes = 350;
  static constexpr uint16_t pm_multicast_message_addr_chars = 351;
};

#endif //__BN_UTILS_H
