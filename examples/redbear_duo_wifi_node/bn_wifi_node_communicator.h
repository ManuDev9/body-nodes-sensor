/**
* MIT License
* 
* Copyright (c) 2021-2025 Manuel Bottini
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

#include "bn_node_specific.h"
#include <utility/imumaths.h>
#include "bn_utils.h"
#include "bn_datatypes.h"

#ifndef __BN__WIFI_NODE_COMMUNICATOR_H__
#define __BN__WIFI_NODE_COMMUNICATOR_H__

#define MAX_MESSAGES_LIST_LENGTH 20
#define MAX_ACTIONS_LIST_LENGTH  20

#define MAX_MESSAGE_BYTES 250
#define MAX_ACTION_BYTES  250

class BnWifiNodeCommunicator {
public:
  BnWifiNodeCommunicator() :
    wnc_messages_doc(MAX_MESSAGES_LIST_LENGTH * MAX_MESSAGE_BYTES),
    wnc_actions_doc(MAX_ACTIONS_LIST_LENGTH * MAX_ACTION_BYTES) {
  }

  void setConnectionParams(JsonObject &params);
  void init();
  bool checkAllOk();
  void addMessage(JsonObject &message);
  void sendAllMessages();
  void getActions(JsonArray &actions);

private:
  void receiveBytes();
  void sendACKN();
  bool checkForACKH();
  void checkForActions();
  void checkStatus();
  bool checkForMulticastBN();
  void saveHostInfo();
  bool hasHostInfo();

  BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_UDP_OBJ wnc_connector;
  BN_NODE_SPECIFIC_BN_WIFI_NODE_COMMUNICATOR_UDP_OBJ wnc_multicast_connector;
  DynamicJsonDocument wnc_messages_doc;
  JsonArray wnc_messages_list;
  DynamicJsonDocument wnc_actions_doc;
  JsonArray wnc_actions_list;

  BnIPConnectionData wnc_connection_data;
  BnIPConnectionData wnc_multicast_data;
  BnStatusLED wnc_status_LED;
};

#endif //__BN__WIFI_NODE_COMMUNICATOR_H__
