/**
* MIT License
* 
* Copyright (c) 2019 Manuel Bottini
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

int sensorPin = A0;
int sensorValue = 0;  // variable to store the value coming from the sensor
float voltage = 0.f;
bool testing = false;
unsigned long startTime;
unsigned long endTime;
float fullChargeVoltage =  4.138;   //lower limit from wireless charging  , 4.154

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);      // open the serial port at 57600 bps:
}

void loop() {

  if (!testing && Serial.available() > 0) {
    if(Serial.read() == 's'){
      testing = true;
      Serial.println("Starting test");
      Serial.println("This test consider that you are starting from a discharged condition");
      Serial.println("and you want to evaluate how to fully charge the battery");
      startTime = millis();
      Serial.print("startTime: ");
      Serial.println(startTime);
    }
  }

  if(testing){
    // put your main code here, to run repeatedly:
    sensorValue = analogRead(sensorPin);
    
    voltage=((float)sensorValue ) / 1023.f * 5.f; //8 is the offset
    
    Serial.print("Valued read: ");       // prints a label
    Serial.println(voltage, 4);

    if(voltage >= fullChargeVoltage ){
      Serial.println("Stopping test");
      testing = false;
      endTime = millis();    
      Serial.print("endTime: ");
      Serial.println(endTime);

      unsigned long diffTime = endTime - startTime;
      unsigned long numberMillis = diffTime % 1000;
      diffTime = diffTime / 1000;
      unsigned long numberSeconds = diffTime % 60;
      diffTime = diffTime / 60;
      unsigned long numberMinutes = diffTime % 60;
      diffTime = diffTime / 60;
      unsigned long numberHours = diffTime % 24;
      unsigned long numberDays = diffTime / 24;

      Serial.println("Charging time:");
      Serial.print("numberDays: ");
      Serial.println(numberDays);
      Serial.print("numberHours: ");
      Serial.println(numberHours);
      Serial.print("numberMinutes: ");
      Serial.println(numberMinutes);
      Serial.print("numberSeconds: ");
      Serial.println(numberSeconds);
      Serial.print("numberMillis: ");
      Serial.println(numberMillis);      
    }
    delay(5000);
  }
}
