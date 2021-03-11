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

#include "messages_mgr.h"
#include <utility/imumaths.h>

Connection mConnections[MAX_BODYNODES_NUMBER];
char mBodyparts[MAX_BODYNODES_NUMBER][MAX_BODYPART_LENGTH];
bool mEnabled[MAX_BODYNODES_NUMBER];

// WS -> Data from Nodes to Main Host
char mMessagesWS_Type[MAX_BODYNODES_NUMBER][MAX_TYPE_LENGTH];
char mMessagesWS_Value[MAX_BODYNODES_NUMBER][MAX_VALUE_LENGTH];
bool mMessagesWS_Changed[MAX_BODYNODES_NUMBER];

/*
 * Initialize and ensure mBodynodes_messages and mBodynodes are empty
 */
void initMessages(){
  for(unsigned int index = 0; index < MAX_BODYNODES_NUMBER; ++index){
    mBodyparts[index][0] = '\0';
    mEnabled[index] = false;
    mMessagesWS_Type[index][0] = '\0';
    mMessagesWS_Value[index][0] = '\0';
    mMessagesWS_Changed[index] = false;
    mConnections[index] = Connection();
  }
}

/*
 * Given a bodypart string, the bodypart index will be returned
 */
int get_index_bodypart(Connection connection, const char *bodypart){
  if(bodypart == nullptr) {
    return -1;
  }
  unsigned int index = 0;
  for( ;index < MAX_BODYNODES_NUMBER && mEnabled[index]; ++index){
    if(strstr(mBodyparts[index], bodypart)!=NULL){
      return index;
    }
  }
  // If not found create new one
  strcpy(mBodyparts[index], bodypart);
  mConnections[index] = connection;
  mEnabled[index] = true;
  return index;
}

Connection get_connection_bodypart(const char *bodypart){
  Connection connection;
  connection.remote_port = 0;
  if(bodypart == nullptr) {
    return connection;
  }
  unsigned int index = 0;
  for( ;index < MAX_BODYNODES_NUMBER && mEnabled[index]; ++index){
    if(strstr(mBodyparts[index], bodypart)!=NULL){
      return mConnections[index];
    }
  }
  return connection;  
}

Connections get_all_connections(){
  Connections connections;
  unsigned int index = 0;
  for(;mEnabled[index]; ++index) {
    connections.bodypart[index] = mConnections[index];
  }
  connections.num_connections = index;
  return connections;
}

void store_message(int index_bodypart, const char *mtype, const char *mvalue){
  if(index_bodypart < 0 || MAX_BODYNODES_NUMBER <= index_bodypart ){
    DEBUG_PRINT("Invalid index_bodypart in store_message");
    return;
  }
  strcpy(mMessagesWS_Type[index_bodypart], mtype);
  strcpy(mMessagesWS_Value[index_bodypart], mvalue);
  mMessagesWS_Changed[index_bodypart] = true;
}

void insertInOrientationStringQuat(char buf[], int startPos, int prec,double quatVal){
  if(quatVal>=0){
    buf[startPos] = '+';
  } else {
    buf[startPos] = '-';
    quatVal=-quatVal;
  }
  
  for(int i=0; i<=prec+1;i++){
    if(i==1){
      buf[startPos+1+i]= '.';
    } else {
      int val = quatVal;
      buf[startPos+1+i]= val+'0';
      quatVal = quatVal-val;
      quatVal*=10;
    }
  }
  buf[startPos+3+prec]= '|';
}

void store_message_quat(int index_bodypart, const char *mtype, imu::Quaternion quat){
  double valW = quat.w();
  double valX = quat.x();
  double valY = quat.y();
  double valZ = quat.z();

  int prec = 3;
  int chars_per_val = prec+4;
  /*
  char buf [chars_per_val*4]={
    '+','0','.','0','0','0','0','|',
    '+','0','.','0','0','0','0','|',
    '+','0','.','0','0','0','0','|',
    '+','0','.','0','0','0','0','\0'};
    */
  char buf [chars_per_val*4];

  insertInOrientationStringQuat(buf,0,prec,valW);
  insertInOrientationStringQuat(buf,chars_per_val,prec,valX);
  insertInOrientationStringQuat(buf,chars_per_val*2,prec,valY);
  insertInOrientationStringQuat(buf,chars_per_val*3,prec,valZ);
  buf[chars_per_val*4-1] = '\0';

  store_message(index_bodypart, mtype, buf);
}

String getAllMessages(){
  String fullMessage ="[";
  bool is_first = true;
  for(unsigned int index = 0;index < MAX_BODYNODES_NUMBER && mEnabled[index]; ++index){
    if(mMessagesWS_Changed[index] == true) {
      if(!is_first){
        fullMessage +=",";        
      }
      is_first = false;
      fullMessage +="{\"bodypart\":\"";
      fullMessage +=mBodyparts[index];
      fullMessage +="\",\"type\":\"";
      fullMessage +=mMessagesWS_Type[index];
      fullMessage +="\",\"value\":\"";
      fullMessage += mMessagesWS_Value[index];
      fullMessage += "\"}";
      mMessagesWS_Changed[index] = false;
    }
  }
  fullMessage += "]";
  return fullMessage;
}
