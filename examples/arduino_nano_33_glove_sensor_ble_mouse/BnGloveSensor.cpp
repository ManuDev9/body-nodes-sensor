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

#include "BnGloveSensor.h"

#ifdef __BN_GLOVE_SENSOR_H__

#define MIGNOLO   0
#define ANULARE   1
#define MEDIO     2
#define INDICE    3
#define POLLICE   4

static int sensorPin[5];
static int digitalPin[4];

static int sensorValueMax[5];
static int sensorValueMin[5];
static int sensorValue[5];
static int digitalValue[4];

#define TOT_READS 20
static int numReads = 0;

// Applying filtering
#define FILTER_SIZE 5

static int filterSensorValues[5][FILTER_SIZE];
static int filterDigitalValues[4][FILTER_SIZE];

static void initFilter(){
    // First value will be a -1, indicating that the filter has to be filled up
    for(uint8_t finger = 0; finger < 5; ++finger){
        filterSensorValues[finger][0] = -1;
    }
    for(uint8_t finger = 0; finger < 4; ++finger){
        filterDigitalValues[finger][0] = -1;
    }
}

static int filterSensorValue(uint8_t finger, int readSensorValue){
    if(filterSensorValues[finger][0] == -1){
        for(uint8_t filter_ix = 0; filter_ix < FILTER_SIZE; ++filter_ix){
            filterSensorValues[finger][filter_ix] = readSensorValue;
        }
        return readSensorValue;
    }
    long sumVal = 0;
    for(uint8_t filter_ix = 0; filter_ix < FILTER_SIZE-1; ++filter_ix){
        filterSensorValues[finger][filter_ix] = filterSensorValues[finger][filter_ix+1];
        sumVal += filterSensorValues[finger][filter_ix];
    }
    filterSensorValues[finger][FILTER_SIZE-1] = readSensorValue;
    sumVal += readSensorValue;
    return sumVal/FILTER_SIZE;    
}

static int filterDigitalValue(uint8_t finger, int readSensorValue){
  
    if(filterDigitalValues[finger][0] == -1){
        for(uint8_t filter_ix = 0; filter_ix < FILTER_SIZE; ++filter_ix){
            filterDigitalValues[finger][filter_ix] = readSensorValue;
        }
        return readSensorValue;
    }
    
    long sumVal = 0;
    for(uint8_t filter_ix = 0; filter_ix < FILTER_SIZE-1; ++filter_ix){
        filterDigitalValues[finger][filter_ix] = filterDigitalValues[finger][filter_ix+1];
        sumVal += filterDigitalValues[finger][filter_ix];
    }
    filterDigitalValues[finger][FILTER_SIZE-1] = readSensorValue;
    sumVal += readSensorValue;
    if(sumVal > (FILTER_SIZE+1)/2 ) {
        return 1;
    } else {
        return 0;
    }
}

void BnGloveSensor::init() {

    sensorPin[MIGNOLO] = A4;
    sensorPin[ANULARE] = A3;
    sensorPin[MEDIO] = A2;
    sensorPin[INDICE] = A1;
    sensorPin[POLLICE] = A0;

    digitalPin[MIGNOLO] = D2;
    digitalPin[ANULARE] = D3;
    digitalPin[MEDIO] = D4;
    digitalPin[INDICE] = D5;

    for(uint8_t finger=0;finger<5;++finger) {
        sensorValue[finger] = 0;
        sensorValueMax[finger] = -1;
        sensorValueMin[finger] = -1;
    }
    initFilter();
    digitalValue[MIGNOLO] = 0;
    digitalValue[ANULARE] = 0;
    digitalValue[MEDIO] = 0;
    digitalValue[INDICE] = 0;

    pinMode(digitalPin[MIGNOLO], INPUT);
    pinMode(digitalPin[ANULARE], INPUT);
    pinMode(digitalPin[MEDIO], INPUT);
    pinMode(digitalPin[INDICE], INPUT);

    gs_enabled = true;
}

bool BnGloveSensor::checkAllOk() {
    for(uint8_t finger=0;finger<5;++finger) {
  
        int tmp = filterSensorValue(finger, analogRead(sensorPin[finger]));
        if(sensorValueMax[finger] == -1 || tmp > sensorValueMax[finger] ) {
            sensorValueMax[finger] = tmp+1;
        }
        if(numReads < TOT_READS) {
            sensorValueMin[finger] = tmp;
        } else if ( tmp < sensorValueMin[finger] ) {
            sensorValueMin[finger] = tmp;
        }
        if(sensorValue[finger] < 0){
            sensorValue[finger] = 0; 
        }
        if( sensorValueMax[finger] != sensorValueMin[finger] ) {
            sensorValue[finger] = (int)((((long)(tmp - sensorValueMin[finger]))*90))/((long)(sensorValueMax[finger]-sensorValueMin[finger]));
        } else {
            sensorValue[finger] = 0;
        }
    }
    if(numReads < TOT_READS) {
        numReads++;
    }
    digitalValue[MIGNOLO] = filterDigitalValue(MIGNOLO, digitalRead(digitalPin[MIGNOLO]));
    digitalValue[ANULARE] = filterDigitalValue(ANULARE, digitalRead(digitalPin[ANULARE]));
    digitalValue[MEDIO] = filterDigitalValue(MEDIO, digitalRead(digitalPin[MEDIO]));
    digitalValue[INDICE] = filterDigitalValue(INDICE, digitalRead(digitalPin[INDICE]));
    return true;
}

// 9 values are expected
void BnGloveSensor::getData(int *values){

    values[0] = sensorValue[MIGNOLO];
    values[1] = sensorValue[ANULARE];
    values[2] = sensorValue[MEDIO];
    values[3] = sensorValue[INDICE];
    values[4] = sensorValue[POLLICE];

    values[5] = digitalValue[MIGNOLO];
    values[6] = digitalValue[ANULARE];
    values[7] = digitalValue[MEDIO];
    values[8] = digitalValue[INDICE];

}

String BnGloveSensor::getType(){
    return SENSORTYPE_GLOVE_TAG;
}

void BnGloveSensor::setEnable(bool enable_status){
    gs_enabled = enable_status;
}

bool BnGloveSensor::isEnabled(){
    return gs_enabled;
}

#endif /*__BN_GLOVE_SENSOR_H__*/
