/**
* MIT License
*
* Copyright (c) 2021-2026 Manuel Bottini
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

#include "BnGloveSensorReaderSerial.h"

#ifdef __BN_GLOVE_SENSOR_READER_SERIAL_H__

void BnGloveSensorReaderSerial::init() {
    Serial.begin(115200);
    while (!Serial) {
        delay (1000);
    }
    grs_lineDone = false;
    grs_lineToPrint = "";
    grs_enabled = true;
}

bool BnGloveSensorReaderSerial::checkAllOk() {
    while(Serial.peek() != -1 && grs_lineDone == false) {
        uint8_t byteVal = Serial.read();

        // In ASCII encoding, \n is the Newline character 0x0A (decimal 10), \r is the Carriage Return character 0x0D (decimal 13).
        if(byteVal == 13 || byteVal == 10) {
            grs_lineDone = true;
        } else {
            grs_lineToPrint += String((char)(byteVal));
        }
        Serial.flush();
    }
    return grs_lineDone;
}

// 9 values are expected
void BnGloveSensorReaderSerial::getData(int *values){
    if(grs_lineDone == false){
        return;
    }
    const int str_len = grs_lineToPrint.length() + 1;
    char buf[str_len];
    grs_lineToPrint.toCharArray(buf, str_len);
    grs_lineToPrint = "";
    grs_lineDone = false;

    char *p_end = &buf[0];
    for(uint8_t counter = 0; counter < 9; ++counter) {
        values[counter] = (int)(strtol(p_end, &p_end, 10));
    }
}

String BnGloveSensorReaderSerial::getType(){
    return BN_SENSORTYPE_GLOVE_TAG;
}

void BnGloveSensorReaderSerial::setEnable(bool enable_status){
    grs_enabled = enable_status;
}

bool BnGloveSensorReaderSerial::isEnabled(){
    return grs_enabled;
}

#endif /*__BN_GLOVE_SENSOR_READER_SERIAL_H__*/
