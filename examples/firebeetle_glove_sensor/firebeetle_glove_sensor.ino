/**
* MIT License
* 
* Copyright (c) 2024-2025 Manuel Bottini
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

/*
  Firebeetle BLE 4.1 Module
  Board: Arduino Pro or Pro Mini
  Processor: ATmega328P (3.3V, 8MHz)
  Bodynodes Gloves 
*/

#define MIGNOLO 0
#define ANULARE 1
#define MEDIO   2
#define INDICE  3
#define POLLICE 4

int sensorPin[5];
int digitalPin[4];

int sensorValueMax[5];
int sensorValueMin[5];
int sensorValue[5];
int digitalValue[4];

#define TOT_READS 20
int numReads = 0;

// Applying filtering
#define FILTER_SIZE 5

int filterSensorValues[5][FILTER_SIZE];
int filterDigitalValues[4][FILTER_SIZE];

int initFilter(){
  // First value will be a -1, indicating that the filter has to be filled up
  for(uint8_t finger = 0; finger < 5; ++finger){
    filterSensorValues[finger][0] == -1;
  }
  for(uint8_t finger = 0; finger < 4; ++finger){
    filterDigitalValues[finger][0] == -1;
  }
}

int filterSensorValue(uint8_t finger, int readSensorValue){
  if(filterSensorValues[finger][0] == -1){
    for(uint8_t filter_ix = 0; filter_ix < FILTER_SIZE; ++filter_ix){
      filterSensorValues[finger][filter_ix] == readSensorValue;
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

int filterDigitalValue(uint8_t finger, int readSensorValue){
  if(filterDigitalValues[finger][0] == -1){
    for(uint8_t filter_ix = 0; filter_ix < FILTER_SIZE; ++filter_ix){
      filterDigitalValues[finger][filter_ix] == readSensorValue;
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


void setup() {
  Serial.begin(115200);

  sensorPin[MIGNOLO] = A0;
  sensorPin[ANULARE] = A1;
  sensorPin[MEDIO] = A2;
  sensorPin[INDICE] = A3;
  sensorPin[POLLICE] = A4;

  digitalPin[MIGNOLO] = 4;
  digitalPin[ANULARE] = 5;
  digitalPin[MEDIO] = 6;
  digitalPin[INDICE] = 7;

  for(uint8_t finger=0;finger<5;++finger) {
    sensorValue[finger] = 0;
    sensorValueMax[finger] = -1;
    sensorValueMin[finger] = -1;
  }

  digitalValue[MIGNOLO] = 0;
  digitalValue[ANULARE] = 0;
  digitalValue[MEDIO] = 0;
  digitalValue[INDICE] = 0;

  pinMode(digitalPin[MIGNOLO], INPUT);
  pinMode(digitalPin[ANULARE], INPUT);
  pinMode(digitalPin[MEDIO], INPUT);
  pinMode(digitalPin[INDICE], INPUT);
}

void loop() {

  for(uint8_t finger=0;finger<5;++finger) {
    int tmp = filterSensorValue(finger, analogRead(sensorPin[finger]));
    if(sensorValueMax[finger] == -1) {
      sensorValueMax[finger] = tmp+1;
    } else if( tmp > sensorValueMax[finger] ) {
      sensorValueMax[finger] = tmp;
    }
    if(numReads < TOT_READS) {
      sensorValueMin[finger] = tmp;
    // } else if ( tmp < sensorValueMinMin[finger] ) {
    //   sensorValueMinMin[finger] = tmp;
    }
    sensorValue[finger] = (int)((((long)(tmp - sensorValueMin[finger]))*90)/((long)(sensorValueMax[finger]-sensorValueMin[finger])));
    if(sensorValue[finger] < 0){
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

  String lineToSend = String(sensorValue[MIGNOLO]) + " " +
    String(sensorValue[ANULARE]) + " " +
    String(sensorValue[MEDIO]) + " " +
    String(sensorValue[INDICE]) + " " +
    String(sensorValue[POLLICE]) + " " +
    String(digitalValue[MIGNOLO]) + " " +
    String(digitalValue[ANULARE]) + " " +
    String(digitalValue[MEDIO]) + " " +
    String(digitalValue[INDICE]) + "\n";
  Serial.print(lineToSend);
  Serial.flush();
  
  delay(30);
}
