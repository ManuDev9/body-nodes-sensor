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

/*
  Redbear DUO; Native USB Port
  Bodynodes Gloves 
*/

#define MIGNOLO 0
#define ANULARE 1
#define MEDIO   2
#define INDICE  3
#define POLLICE 4


#if defined(ARDUINO)
SYSTEM_MODE(MANUAL);
#endif


int sensorPin[5];
int digitalPin[4];

int sensorValue[5];
int digitalValue[4];

void setup() {
  Serial.begin(921600);

  sensorPin[MIGNOLO] = A0;
  sensorPin[ANULARE] = A1;
  sensorPin[MEDIO] = A2;
  sensorPin[INDICE] = A3;
  sensorPin[POLLICE] = A4;

  digitalPin[MIGNOLO] = D0;
  digitalPin[ANULARE] = D1;
  digitalPin[MEDIO] = D2;
  digitalPin[INDICE] = D3;
  
  digitalValue[MIGNOLO] = 0;
  digitalValue[ANULARE] = 0;
  digitalValue[MEDIO] = 0;
  digitalValue[INDICE] = 0;

  pinMode(digitalPin[MIGNOLO], INPUT);
  pinMode(digitalPin[ANULARE], INPUT);
  pinMode(digitalPin[MEDIO], INPUT);
  pinMode(digitalPin[INDICE], INPUT);}

void loop() {

  for(uint8_t finger=0;finger<5;++finger) {
    int tmp =  analogRead(sensorPin[finger]);
    sensorValue[finger] = tmp;
  }


  digitalValue[MIGNOLO] = digitalRead(digitalPin[MIGNOLO]);
  digitalValue[ANULARE] = digitalRead(digitalPin[ANULARE]);
  digitalValue[MEDIO] = digitalRead(digitalPin[MEDIO]);
  digitalValue[INDICE] = digitalRead(digitalPin[INDICE]);

  String lineToSend = String(sensorValue[MIGNOLO]) + ", " +
    String(sensorValue[ANULARE]) + " " +
    String(sensorValue[MEDIO]) + " " +
    String(sensorValue[INDICE]) + " " +
    String(sensorValue[POLLICE]) + " " +
    String(digitalValue[MIGNOLO]) + " " +
    String(digitalValue[ANULARE]) + " " +
    String(digitalValue[MEDIO]) + " " +
    String(digitalValue[INDICE]) + "\n";
  Serial.print(lineToSend);
  //delay(1000);

  delay(30);
}
