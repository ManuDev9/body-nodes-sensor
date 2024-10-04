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
#  "board" : "esp-12e",
#  "node_communicator": "wifi",
#  "sensors": {
#    "acceleration_rel" : "no",         # Possible values: "no"
#    "orientation_abs" : "onboard",     # Possible values: "no", "onboard"
#    "glove" : "serial",                # Possible values: "no", "serial", "onboard"
#    "shoe" : "onboard"                 # Possible values: "no", "onboard"
#  }
#}


def enable_field_in_file( full_file_path, field ):
    with open(full_file_path, 'r') as file:
        file_content = file.read()
    modified_content = file_content.replace(
            "// #define "+field,
            "#define "+field )
    with open(full_file_path, 'w') as file:
        file.write(modified_content)

def main_node(project_path, config_json):

    files_to_take = []

    if project_path.endswith('/'):
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
    if config_json["sensors"]["orientation_abs"] == "onboard":
        files_to_take.append(template_node_sensors_folder+"BnOrientationAbsSensor.cpp")
        files_to_take.append(template_node_sensors_folder+"BnOrientationAbsSensor.h")

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
    else:
        print("Invalid 'node_communicator' = "+config_json["node_communicator"])
        return

    # Board
    if config_json["board"] == "esp-12e":
        template_board_folder = "templates/node_specific/esp-12e/"
        files_to_take.append(template_board_folder+"BnNodeSpecific.cpp")
        files_to_take.append(template_board_folder+"BnNodeSpecific.h")
    elif config_json["board"] == "redbear_duo":
        template_board_folder = "templates/node_specific/redbear_duo/"
        files_to_take.append(template_board_folder+"BnNodeSpecific.cpp")
        files_to_take.append(template_board_folder+"BnNodeSpecific.h")
    elif config_json["board"] == "custom":
        template_board_folder = "templates/node_specific/custom/"
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

        full_file_path = project_path+"/"+file_name
        shutil.copy( file_to_take, full_file_path )

        if file_name == "BnNodeSpecific.h":
            if config_json["node_communicator"] == "wifi":
                enable_field_in_file( full_file_path, "WIFI_COMMUNICATION" )

            if config_json["sensors"]["orientation_abs"] == "onboard":
                enable_field_in_file( full_file_path, "ORIENTATION_ABS_SENSOR_ON_BOARD" )

            if config_json["sensors"]["glove"] == "serial":
                enable_field_in_file( full_file_path, "GLOVE_SENSOR_ON_SERIAL" )

            if config_json["sensors"]["glove"] == "onboard":
                enable_field_in_file( full_file_path, "GLOVE_SENSOR_ON_BOARD" )

            if config_json["sensors"]["shoe"] == "onboard":
                enable_field_in_file( full_file_path, "SHOE_SENSOR_ON_BOARD" )

            if config_json["actuators"]["haptic"] == "yes":
                enable_field_in_file( full_file_path, "HAPTIC_ACTUATOR_ON_BOARD" )



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




