
#include "Adafruit_MPU6050.h"
#include <Wire.h>

#define DPIN_SDA 30
#define DPIN_SCL 1

TwoWire MPU6050Wire(NRF_TWIM0, NRF_TWIS0, SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn, DPIN_SDA, DPIN_SCL);

// Create an MPU6050 object
Adafruit_MPU6050 mpu;

void setup() {

  // Initialize serial communication
  Serial.begin(115200);
  MPU6050Wire.begin(); 
  if (!mpu.begin(MPU6050_I2CADDR_DEFAULT, &MPU6050Wire)) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }
  Serial.println("MPU6050 found!");

}



void loop() {

  sensors_event_t a, g, temp;
  mpu.getAccelerometerSensor()->getEvent(&a);
  mpu.getGyroSensor()->getEvent(&g);
  mpu.getTemperatureSensor()->getEvent(&temp);

  Serial.print("Accel X: ");
  Serial.print(a.acceleration.x);
  Serial.print(" m/s^2, Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(" m/s^2, Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Gyro X: ");
  Serial.print(g.gyro.x);
  Serial.print(" rad/s, Y: ");
  Serial.print(g.gyro.y);
  Serial.print(" rad/s, Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temp: ");
  Serial.print(temp.temperature);
  Serial.println(" C");
  delay(2000);
}
/*
#include <bluefruit.h>

// nice!nano v1  https://nicekeyboards.com/docs/nice-nano/pinout-schematic  
// https://infocenter.nordicsemi.com/pdf/nRF52840_PS_v1.1.pdf
// Go to Tools > Board and select Adafruit Feather nRF52840 Express (this works for the Nice!Nano as they share the same core).

// 0x40004000 TWI TWI1 Two-wire interface master 1
// 0x40003000 TWI TWI0 Two-wire interface master 0
// PSEL.SCL 0x508 Pin select for SCL
// PSEL.SDA 0x50C Pin select for SDA
// In here I can find more info no the variant C:\Users\Manu\AppData\Local\Arduino15\packages\adafruit\hardware\nrf52\1.6.1\variants\feather_nrf52840_express
// I changed variant.cpp
//  22,  //  P0.22 (SDA)
//  24,  //  P0.24 (SCL)
//  24,  // D1  is P0.24 (UART RX 
//  22,  // D30 is P0.22 (QSPI Data 1)
//Serial.println(*(uint32_t *)(0x40003508), BIN); //11000 -> port 0.24 -> SCL
//Serial.println(*(uint32_t *)(0x4000350C), BIN); //10110 -> port 0.22 -> SDA

// A RW PIN [0..31] Pin number
// B RW PORT [0..1] Port number
// C RW CONNECT Connection

BLEService randomService = BLEService(0x180F);  // Create a random service with a UUID (0x180F = Battery Service, using it for demo)
BLECharacteristic randomCharacteristic = BLECharacteristic(0x2A19);  // Random characteristic (0x2A19 = Battery Level)

void setup() {
  // Initialize the Bluefruit module
  Bluefruit.begin();
  
  // Set the device name and appearance
  Bluefruit.setName("Nice!Nano_Random_Service");
  Bluefruit.setTxPower(4); // Max power
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  
  // Start BLE advertising
  startAdv();

  // Create the service and characteristic
  randomService.begin();
  
  randomCharacteristic.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY); // Read and notify properties
  randomCharacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);  // Permissions
  randomCharacteristic.setFixedLen(1);  // 1-byte data for the characteristic (e.g., a random value between 0-255)
  randomCharacteristic.begin();

  Serial.begin(115200);
  Serial.println("BLE Random Service Example");
}

void loop() {
  // Generate a random value between 0 and 255
  uint8_t randomValue = random(0, 256);

  // Update the characteristic with the new value
  randomCharacteristic.write8(randomValue);

  // Notify connected device (if any)
  randomCharacteristic.notify(&randomValue, sizeof(randomValue));

  Serial.print("Sent random value: ");
  Serial.println(randomValue);

  delay(1000);  // Send new value every 1 second
}

// Function to start advertising
void startAdv(void) {
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  
  // Include the service in the advertising data
  Bluefruit.Advertising.addService(randomService);

  // Include the device name in the scan response
  Bluefruit.ScanResponse.addName();

  // Start advertising indefinitely
  Bluefruit.Advertising.start(0);
}

// Callback when connected
void connect_callback(uint16_t conn_handle) {
  Serial.println("Connected");
}

// Callback when disconnected
void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  Serial.print("Disconnected, reason: ");
  Serial.println(reason);

  // Restart advertising after disconnection
  startAdv();
}
*/
