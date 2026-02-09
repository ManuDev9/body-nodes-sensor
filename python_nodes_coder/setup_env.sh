#!/bin/bash

# curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
# pip3 install adafruit-nrfutil --break-system-packages
# Config file written to: $HOME/.arduino15/arduino-cli.yaml

arduino-cli config init
arduino-cli config add board_manager.additional_urls "https://redbearlab.github.io/arduino/package_redbear_index.json"
arduino-cli config add board_manager.additional_urls "http://arduino.esp8266.com/stable/package_esp8266com_index.json"
arduino-cli config add board_manager.additional_urls "https://adafruit.github.io/arduino-board-index/package_adafruit_index.json"
arduino-cli config set directories.user "$HOME/Arduino"
arduino-cli config set directories.data "$HOME/.arduino15"
arduino-cli core update-index; sed -i 's/"boards": {"name": "RedBear Duo"}/"boards": [ {"name": "RedBear Duo"} ]/' $HOME/.arduino15/package_redbear_index.json

arduino-cli lib install "ArduinoJson@6.15.2"
arduino-cli lib install "Adafruit Unified Sensor@1.1.15"
arduino-cli lib install "Adafruit BNO055@1.6.4"
arduino-cli lib install "Adafruit BusIO@1.17.4"
arduino-cli lib install "Adafruit SSD1306@2.5.16"
arduino-cli lib install "Adafruit GFX Library@1.12.4"
arduino-cli lib install "Adafruit MPU6050@2.2.8"
arduino-cli lib install "Arduino_LSM9DS1@1.1.1"
arduino-cli lib install "NanoBLEFlashPrefs@1.2.0"
arduino-cli lib install "ArduinoBLE@1.5.0"


arduino-cli core install RedBear:STM32F2@0.3.3
arduino-cli core install esp8266:esp8266@3.1.2
arduino-cli core install arduino:mbed_nano@4.5.0
arduino-cli core install adafruit:nrf52@1.7.0



