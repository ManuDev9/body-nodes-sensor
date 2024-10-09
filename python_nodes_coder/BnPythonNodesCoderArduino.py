#!/usr/bin/python3

#
# MIT License
# 
# Copyright (c) 2024 Manuel Bottini
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#


import shutil
import sys
import os
import json

# Example of JSON Config file:
#{
#  "type" : "node",
#  "board" : "esp-12e",                 # Possible values: "esp-12e", "arduino_nano_33", "redbear_duo", "mpnrf52840"
#  "node_communicator": "wifi",         # Possible values: "wifi", "ble"
#  "sensors": {
#    "acceleration_rel" : "no",         # Possible values: "no"
#    "orientation_abs" : "bno055",      # Possible values: "no", "bno055", "arduino_lsm9ds1_sf", "mpu6050"
#    "glove" : "serial",                # Possible values: "no", "serial", "onboard"
#    "shoe" : "onboard"                 # Possible values: "no", "onboard"
#  }
#}


def add_field_in_file( full_file_path, type, field ):
    with open(full_file_path, 'r') as file:
        file_content = file.read()
        
    tag = "// "+ type +" //"    
    
    modified_content = file_content.replace(
            tag,
            tag+"\n"+"#define "+field )
    with open(full_file_path, 'w') as file:
        file.write(modified_content)

def add_include_in_file( full_file_path, type, field ):
    with open(full_file_path, 'r') as file:
        file_content = file.read()
        
    tag = "// "+ type +" //"    
    
    modified_content = file_content.replace(
            tag,
            tag+"\n"+"#include \""+field +"\"" )
    with open(full_file_path, 'w') as file:
        file.write(modified_content)


def main_node(project_path, config_json):

    files_to_take = []

    project_path = os.path.normpath(project_path) 
    if project_path.endswith(os.path.sep):  # os.path.sep is '/' on Unix and '\' on Windows
        project_path = project_path[:-1]

    # Type files
    template_type_folder = "templates/"+config_json["type"]+"/"
    if not os.path.exists(template_type_folder):
        print("Invalid 'type' = "+config_json["type"]+ " in bn_coder_config.json")
        return
    
    files_to_take.append(template_type_folder+"BnDatatypes.cpp")
    files_to_take.append(template_type_folder+"BnDatatypes.h")
    files_to_take.append(template_type_folder+"BnArduinoUtils.cpp")
    files_to_take.append(template_type_folder+"BnArduinoUtils.h")
    files_to_take.append(template_type_folder+"bodynode.ino")

    # Actuators files
    template_node_actuators_folder = "templates/actuators/"
    if config_json["actuators"]["haptic"] == "yes":
        files_to_take.append(template_node_actuators_folder+"BnHapticActuator.cpp")
        files_to_take.append(template_node_actuators_folder+"BnHapticActuator.h")

    # Sensors files
    template_node_sensors_folder = "templates/sensors/"
    orientation_abs_sensor_header = None
    if config_json["sensors"]["orientation_abs"] == "bno055":
        files_to_take.append(template_node_sensors_folder+"BnOrientationAbsSensor_BNO055.cpp")
        files_to_take.append(template_node_sensors_folder+"BnOrientationAbsSensor_BNO055.h")
        orientation_abs_sensor_header = "BnOrientationAbsSensor_BNO055.h"
    elif config_json["sensors"]["orientation_abs"] == "arduino_lsm9ds1_sf":
        files_to_take.append(template_node_sensors_folder+"BnOrientationAbsSensorFusion_ArduinoLSM9DS1.cpp")
        files_to_take.append(template_node_sensors_folder+"BnOrientationAbsSensorFusion_ArduinoLSM9DS1.h")
        orientation_abs_sensor_header = "BnOrientationAbsSensorFusion_ArduinoLSM9DS1.h"
    elif config_json["sensors"]["orientation_abs"] == "mpu6050_sf":
        files_to_take.append(template_node_sensors_folder+"BnOrientationAbsSensorFusion_MPU6050.cpp")
        files_to_take.append(template_node_sensors_folder+"BnOrientationAbsSensorFusion_MPU6050.h")
        orientation_abs_sensor_header = "BnOrientationAbsSensorFusion_MPU6050.h"
    
    if config_json["sensors"]["glove"] == "serial":
        files_to_take.append(template_node_sensors_folder+"BnGloveSensorReaderSerial.cpp")
        files_to_take.append(template_node_sensors_folder+"BnGloveSensorReaderSerial.h")

    if config_json["sensors"]["glove"] == "onboard":
        files_to_take.append(template_node_sensors_folder+"BnGloveSensor.cpp")
        files_to_take.append(template_node_sensors_folder+"BnGloveSensor.h")

    if config_json["sensors"]["shoe"] == "onboard":
        files_to_take.append(template_node_sensors_folder+"BnShoeSensor.cpp")
        files_to_take.append(template_node_sensors_folder+"BnShoeSensor.h")

    # Common files
    template_common_folder = "../body-nodes-common/cpp/"
    files_to_take.append(template_common_folder+"BnConstants.h")

    # Node communicator
    template_node_communicator_folder = "templates/node_communicators/"
    if config_json["node_communicator"] == "wifi":
        files_to_take.append(template_node_communicator_folder+"BnWifiNodeCommunicator.cpp")
        files_to_take.append(template_node_communicator_folder+"BnWifiNodeCommunicator.h")
    elif config_json["node_communicator"] == "ble":
        files_to_take.append(template_node_communicator_folder+"BnBLENodeCommunicator.cpp")
        files_to_take.append(template_node_communicator_folder+"BnBLENodeCommunicator.h")
    else:
        print("Invalid 'node_communicator' = "+config_json["node_communicator"])
        return

    # Board
    template_board_folder = "templates/node_specific/"+config_json["board"] +"/"
    if os.path.exists(template_board_folder) and os.path.isdir(template_board_folder):
        files_to_take.append(template_board_folder+"BnNodeSpecific.cpp")
        files_to_take.append(template_board_folder+"BnNodeSpecific.h")
    else:
        print("Invalid 'board' = "+config_json["board"])
        return

    for file_to_take in files_to_take:
        file_name = os.path.basename(file_to_take)
        is_bodynodeino = False
        if(file_name == "bodynode.ino"):
            file_name = os.path.basename(project_path)+".ino"
            is_bodynodeino = True

        full_file_path = os.path.join(project_path, file_name)
        shutil.copy( file_to_take, full_file_path )

        if file_name == "BnNodeSpecific.h":
            if config_json["node_communicator"] == "wifi":
                add_field_in_file( full_file_path, "COMMUNICATION" ,"WIFI_COMMUNICATION" )

            if config_json["node_communicator"] == "ble":
                add_field_in_file( full_file_path, "COMMUNICATION", "BLE_COMMUNICATION" )

            if config_json["sensors"]["orientation_abs"] != "no":
                add_field_in_file( full_file_path, "SENSORS", "ORIENTATION_ABS_SENSOR" )

            if config_json["sensors"]["glove"] == "serial":
                add_field_in_file( full_file_path, "SENSORS", "GLOVE_SENSOR_ON_SERIAL" )

            if config_json["sensors"]["glove"] == "onboard":
                add_field_in_file( full_file_path, "SENSORS", "GLOVE_SENSOR_ON_BOARD" )

            if config_json["sensors"]["shoe"] == "onboard":
                add_field_in_file( full_file_path, "SENSORS", "SHOE_SENSOR_ON_BOARD" )

            if config_json["actuators"]["haptic"] == "yes":
                add_field_in_file( full_file_path, "ACTUATORS", "HAPTIC_ACTUATOR_ON_BOARD" )

        if is_bodynodeino:
            if orientation_abs_sensor_header != None:
                add_include_in_file( full_file_path, "ORIENTATION_ABS_SENSOR_HEADER", orientation_abs_sensor_header )
            


def main(project_path):

    # Check if project_path exists and if it is a folder
    if os.path.exists(project_path):
        if os.path.isdir(project_path):
            print("The path "+project_path+" exists and it's a directory.")
        else:
            print("The path "+project_path+" exists but it's not a directory.")
            return
    else:
        print("The path "+project_path+" does not exist.")
        return

    # Check that the bn_coder_config.json exists inside the project file, that it is a json file, and it contains all the required fields
    config_path = project_path+"/bn_coder_config.json"

    # FIle existence
    if os.path.exists(config_path):
        if os.path.isfile(config_path):
            print("The path "+config_path+" exists and it's a file.")
        else:
            print("The path "+config_path+" exists but it's not a file.")
            return
    else:
        print("The path "+config_path+" does not exist.")
        return

    # Read the json
    config_json = None
    try:
        with open(config_path, 'r') as file:
            config_json = json.load(file)
    except json.JSONDecodeError as e:
        print("Error: Invalid JSON format in file "+config_json+": {e}")

    # Check fields
    if "type" not in config_json:
        print("Missing field 'type' in bn_coder_config.json")
        return
    if "board" not in config_json:
        print("Missing field 'board' in bn_coder_config.json")
        return
    if "node_communicator" not in config_json:
        print("Missing field 'node_communicator' in bn_coder_config.json")
        return
    if "sensors" not in config_json:
        print("Missing field 'sensors' in bn_coder_config.json")
        return
    if "acceleration_rel" not in config_json["sensors"]:
        print("Missing field 'acceleration_rel' for 'sensors' in bn_coder_config.json")
        return
    if "orientation_abs" not in config_json["sensors"]:
        print("Missing field 'orientation_abs' for 'sensors' in bn_coder_config.json")
        return
    if "glove" not in config_json["sensors"]:
        print("Missing field 'glove' for 'sensors' in bn_coder_config.json")
        return
    if "shoe" not in config_json["sensors"]:
        print("Missing field 'shoe' for 'sensors' in bn_coder_config.json")
        return

    # There might be additional stuff, and it will just be ignored

    if config_json["type"] == "node":
        print("Creating an Arduino Bodynodes Sensor project")
        main_node(project_path, config_json)
        print("Finished!")


def print_help():
    print("You can run the tool in the following way:")
    print("\t$ python3 bn_coder_arduino.py <project_path>")
    print("\nWhere:")
    print("\t- <project_path> is the path to the folder to the project.")
    print("\t\tThe project should contain a bn_coder_config.json")
    print("\t\tYou will have to change and adapt BnNodeSpecific.h and BnNodeSpecific.cpp to your platform")

if __name__ == "__main__":
    # Setup required arguments
    arguments = sys.argv
    project_path = None

    # Read the given arguments
    if len(arguments) > 1:
        project_path = arguments[1]

    # Check errors in the arguments
    if project_path == None:
        print("<project_path> is missing")
        print_help()
        exit()

    # Call the main function
    main(project_path)




