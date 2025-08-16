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

#include "BnNodeSpecific.h"
#include "BnDatatypes.h"

#ifndef __BN__BLE_NODE_COMMUNICATOR_H__
#define __BN__BLE_NODE_COMMUNICATOR_H__

#define MAX_MESSAGES_LIST_LENGTH 20
#define MAX_ACTIONS_LIST_LENGTH  20

#define MAX_MESSAGE_BYTES 250
#define MAX_ACTION_BYTES  250

#ifdef BLE_COMMUNICATION

class BnBLENodeCommunicator {
public:
    BnBLENodeCommunicator() :
        bnc_messages_doc(MAX_MESSAGES_LIST_LENGTH * MAX_MESSAGE_BYTES) {
    }
    
    void setConnectionParams(JsonObject &params);
    void init();
    bool checkAllOk();
    void addMessage(JsonObject &message);
    void sendAllMessages();
    void getActions(JsonArray &actions);

private:
    void checkStatus();
    
    DynamicJsonDocument bnc_messages_doc;
    JsonArray bnc_messages_list;
    
    BnBLEConnectionData bnc_connection_data;
    BnStatusLED bnc_status_LED;
};

#endif
#endif //__BN__BLE_NODE_COMMUNICATOR_H__
