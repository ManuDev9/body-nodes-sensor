#!/usr/bin/python3
# MIT License
#
# Copyright (c) 2024-2026 Manuel Bottini
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
import argparse
import itertools
import shutil
import subprocess

# Example of JSON Config file:
# {
#  "type" : "node",
#  "board" : "esp-12e",                 # Possible values: "esp-12e", "arduino_nano_33", "redbear_duo", "mpnrf52840"
#  "fqbn" : "xxxx",                     #
#  "node_communicator": "wifi",         # Possible values: "wifi", "ble"
#  "actuators": {
#      "haptic": "no"                   # Possible values: "no", "yes"
#  },
#  "isensors": "mpu6050",               # Possible values: "no", "bno055", "arduino_lsm9ds1", "mpu6050"
#  "esensors": {
#    "acceleration_rel" : "no",         # Possible values: "no", "yes"
#    "angularvelocity_rel" : "no",      # Possible values: "no", "yes"
#    "orientation_abs" : "fusion",      # Possible values: "no", "onboard", "fusion"
#    "glove" : "serial",                # Possible values: "no", "onboard", "serial",
#    "shoe" : "onboard"                 # Possible values: "no", "onboard"
#  }
# }


def add_field_in_file(full_file_path, type, field):
    with open(full_file_path, "r") as file:
        file_content = file.read()

    tag = "// " + type + " //"

    modified_content = file_content.replace(tag, tag + "\n" + "#define " + field)
    with open(full_file_path, "w") as file:
        file.write(modified_content)


def add_include_in_file(full_file_path, type, field):
    with open(full_file_path, "r") as file:
        file_content = file.read()

    tag = "// " + type + " //"

    modified_content = file_content.replace(
        tag, tag + "\n" + '#include "' + field + '"'
    )
    with open(full_file_path, "w") as file:
        file.write(modified_content)


def main_node(project_path, config_json):

    files_to_take = []

    project_path = os.path.normpath(project_path)
    if project_path.endswith(
        os.path.sep
    ):  # os.path.sep is '/' on Unix and '\' on Windows
        project_path = project_path[:-1]

    # Type files
    template_type_folder = "templates/" + config_json["type"] + "/"
    if not os.path.exists(template_type_folder):
        print("Invalid 'type' = " + config_json["type"] + " in bn_coder_config.json")
        return

    files_to_take.append(template_type_folder + "BnDatatypes.cpp")
    files_to_take.append(template_type_folder + "BnDatatypes.h")
    files_to_take.append(template_type_folder + "BnArduinoUtils.cpp")
    files_to_take.append(template_type_folder + "BnArduinoUtils.h")
    files_to_take.append(template_type_folder + "bodynode.ino")

    # Actuators files
    template_node_actuators_folder = "templates/actuators/"
    if config_json["actuators"]["haptic"] == "yes":
        files_to_take.append(template_node_actuators_folder + "BnHapticActuator.cpp")
        files_to_take.append(template_node_actuators_folder + "BnHapticActuator.h")

    # Internal Sensors files
    template_node_isensors_folder = "templates/isensors/"
    files_to_take.append(template_node_isensors_folder + "BnISensor.h")
    if config_json["isensor"] == "bno055":
        files_to_take.append(template_node_isensors_folder + "BnISensorBNO055.cpp")
    elif config_json["isensor"] == "arduino_lsm9ds1":
        files_to_take.append(
            template_node_isensors_folder + "BnISensorArduinoLSM9DS1.cpp"
        )
    elif config_json["isensor"] == "mpu6050":
        files_to_take.append(template_node_isensors_folder + "BnISensorMPU6050.cpp")

    # External Sensors files
    template_node_esensors_folder = "templates/esensors/"
    if config_json["esensors"]["orientation_abs"] == "onboard":
        files_to_take.append(template_node_esensors_folder + "BnOrientationAbsSensor.h")
        files_to_take.append(
            template_node_esensors_folder + "BnOrientationAbsSensorOnBoard.cpp"
        )
    elif config_json["esensors"]["orientation_abs"] == "fusion":
        files_to_take.append(template_node_esensors_folder + "BnOrientationAbsSensor.h")
        files_to_take.append(
            template_node_esensors_folder + "BnOrientationAbsSensorFusion.cpp"
        )

    if config_json["esensors"]["acceleration_rel"] == "yes":
        files_to_take.append(
            template_node_esensors_folder + "BnAccelerationRelSensor.h"
        )
        files_to_take.append(
            template_node_esensors_folder + "BnAccelerationRelSensor.cpp"
        )

    if config_json["esensors"]["angularvelocity_rel"] == "yes":
        files_to_take.append(
            template_node_esensors_folder + "BnAngularVelocityRelSensor.h"
        )
        files_to_take.append(
            template_node_esensors_folder + "BnAngularVelocityRelSensor.cpp"
        )

    if config_json["esensors"]["glove"] == "serial":
        files_to_take.append(
            template_node_esensors_folder + "BnGloveSensorReaderSerial.cpp"
        )
        files_to_take.append(
            template_node_esensors_folder + "BnGloveSensorReaderSerial.h"
        )

    if config_json["esensors"]["glove"] == "onboard":
        files_to_take.append(template_node_esensors_folder + "BnGloveSensor.cpp")
        files_to_take.append(template_node_esensors_folder + "BnGloveSensor.h")

    if config_json["esensors"]["shoe"] == "onboard":
        files_to_take.append(template_node_esensors_folder + "BnShoeSensor.cpp")
        files_to_take.append(template_node_esensors_folder + "BnShoeSensor.h")

    # Common files
    template_common_folder = "../body-nodes-common/cpp/src/"
    files_to_take.append(template_common_folder + "BnConstants.h")

    # Node communicator
    template_node_communicator_folder = "templates/node_communicators/"
    if config_json["node_communicator"] == "wifi":
        files_to_take.append(
            template_node_communicator_folder + "BnWifiNodeCommunicator.cpp"
        )
        files_to_take.append(
            template_node_communicator_folder + "BnWifiNodeCommunicator.h"
        )
    elif config_json["node_communicator"] == "ble":
        files_to_take.append(
            template_node_communicator_folder + "BnBLENodeCommunicator.cpp"
        )
        files_to_take.append(
            template_node_communicator_folder + "BnBLENodeCommunicator.h"
        )
    else:
        print("Invalid 'node_communicator' = " + config_json["node_communicator"])
        return

    # Board
    template_board_folder = "templates/board/" + config_json["board"] + "/"
    if os.path.exists(template_board_folder) and os.path.isdir(template_board_folder):
        files_to_take.append(template_board_folder + "BnNodeSpecific.cpp")
        files_to_take.append(template_board_folder + "BnNodeSpecific.h")
    else:
        print("Invalid 'board' = " + config_json["board"])
        return

    for file_to_take in files_to_take:
        file_name = os.path.basename(file_to_take)
        is_bodynodeino = False
        if file_name == "bodynode.ino":
            file_name = os.path.basename(project_path) + ".ino"
            is_bodynodeino = True

        full_file_path = os.path.join(project_path, file_name)
        shutil.copy(file_to_take, full_file_path)

        if file_name == "BnNodeSpecific.h":
            if config_json["node_communicator"] == "wifi":
                add_field_in_file(full_file_path, "COMMUNICATION", "WIFI_COMMUNICATION")

            if config_json["node_communicator"] == "ble":
                add_field_in_file(full_file_path, "COMMUNICATION", "BLE_COMMUNICATION")

            if config_json["esensors"]["acceleration_rel"] != "no":
                add_field_in_file(full_file_path, "SENSORS", "ACCELERATION_REL_SENSOR")

            if config_json["esensors"]["orientation_abs"] != "no":
                add_field_in_file(full_file_path, "SENSORS", "ORIENTATION_ABS_SENSOR")

            if config_json["esensors"]["angularvelocity_rel"] != "no":
                add_field_in_file(
                    full_file_path, "SENSORS", "ANGULARVELOCITY_REL_SENSOR"
                )

            if config_json["esensors"]["glove"] == "serial":
                add_field_in_file(full_file_path, "SENSORS", "GLOVE_SENSOR_ON_SERIAL")

            if config_json["esensors"]["glove"] == "onboard":
                add_field_in_file(full_file_path, "SENSORS", "GLOVE_SENSOR_ON_BOARD")

            if config_json["esensors"]["shoe"] == "onboard":
                add_field_in_file(full_file_path, "SENSORS", "SHOE_SENSOR_ON_BOARD")

            if config_json["actuators"]["haptic"] == "yes":
                add_field_in_file(
                    full_file_path, "ACTUATORS", "HAPTIC_ACTUATOR_ON_BOARD"
                )

        # if is_bodynodeino:
        #    if orientation_abs_sensor_header != None:
        #        add_include_in_file( full_file_path, "ORIENTATION_ABS_SENSOR_HEADER", orientation_abs_sensor_header )


def main(project_path):

    # Check if project_path exists and if it is a folder
    if os.path.exists(project_path):
        if os.path.isdir(project_path):
            print("The path " + project_path + " exists and it's a directory.")
        else:
            print("The path " + project_path + " exists but it's not a directory.")
            return
    else:
        print("The path " + project_path + " does not exist.")
        return

    # Check that the bn_coder_config.json exists inside the project file, that it is a json file, and it contains all the required fields
    config_path = project_path + "/bn_coder_config.json"

    # FIle existence
    if os.path.exists(config_path):
        if os.path.isfile(config_path):
            print("The path " + config_path + " exists and it's a file.")
        else:
            print("The path " + config_path + " exists but it's not a file.")
            return
    else:
        print("The path " + config_path + " does not exist.")
        return

    # Read the json
    config_json = None
    try:
        with open(config_path, "r") as file:
            config_json = json.load(file)
    except json.JSONDecodeError as e:
        print("Error: Invalid JSON format in file " + config_json + ": {e}")

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
    if "isensor" not in config_json:
        print("Missing field 'isensor' in bn_coder_config.json")
        return
    if "actuators" not in config_json:
        print("Missing field 'actuators' in bn_coder_config.json")
        return
    if "haptic" not in config_json["actuators"]:
        print("Missing field 'haptic' in bn_coder_config.json")
        return
    if "esensors" not in config_json:
        print("Missing field 'esensors' in bn_coder_config.json")
        return
    if "acceleration_rel" not in config_json["esensors"]:
        print("Missing field 'acceleration_rel' for 'sensors' in bn_coder_config.json")
        return
    if "angularvelocity_rel" not in config_json["esensors"]:
        print(
            "Missing field 'angularvelocity_rel' for 'sensors' in bn_coder_config.json"
        )
        return
    if "orientation_abs" not in config_json["esensors"]:
        print("Missing field 'orientation_abs' for 'sensors' in bn_coder_config.json")
        return
    if "glove" not in config_json["esensors"]:
        print("Missing field 'glove' for 'sensors' in bn_coder_config.json")
        return
    if "shoe" not in config_json["esensors"]:
        print("Missing field 'shoe' for 'sensors' in bn_coder_config.json")
        return

    # There might be additional stuff, and it will just be ignored

    if config_json["type"] == "node":
        print("Creating an Arduino Bodynodes Sensor project")
        main_node(project_path, config_json)
        print("Finished!")


def create_combo(flat_keys, value_lists):
    all_configs = []
    for i, combo in enumerate(itertools.product(*value_lists)):
        # Create nested structure
        res = {"actuators": {}, "esensors": {}}

        for key_path, value in zip(flat_keys, combo):
            if len(key_path) == 1:
                res[key_path[0]] = value
            else:
                res[key_path[0]][key_path[1]] = value

        all_configs.append(res)
    return all_configs


def generate_all_configs():

    all_configs = []
    flat_keys = [
        ("type",),
        ("board",),
        ("fqbn",),
        ("node_communicator",),
        ("actuators", "haptic"),
        ("isensor",),
        ("esensors", "acceleration_rel"),
        ("esensors", "angularvelocity_rel"),
        ("esensors", "orientation_abs"),
        ("esensors", "glove"),
        ("esensors", "shoe"),
    ]

    ####### esp-12e

    value_lists = [
        ["node"],  # type
        ["esp-12e"],  # board
        ["esp8266:esp8266:nodemcuv2"],  # fqbn
        ["wifi"],  # node_communicator
        ["yes"],  # actuators->haptic
        ["mpu6050"],  # isensors
        ["yes"],  # esensors->acceleration_rel
        ["yes"],  # esensors->angularvelocity_rel
        ["fusion"],  # esensors->orientation_abs
        ["onboard", "serial"],  # esensors->glove
        ["onboard"],  # esensors->shoe
    ]
    all_configs.extend(create_combo(flat_keys, value_lists))

    value_lists = [
        ["node"],  # type
        ["esp-12e"],  # board
        ["esp8266:esp8266:nodemcuv2"],  # fqbn
        ["wifi"],  # node_communicator
        ["yes"],  # actuators->haptic
        ["bno055"],  # isensors
        ["yes"],  # esensors->acceleration_rel
        ["yes"],  # esensors->angularvelocity_rel
        ["onboard", "fusion"],  # esensors->orientation_abs
        ["no"],  # esensors->glove
        ["no"],  # esensors->shoe
    ]
    all_configs.extend(create_combo(flat_keys, value_lists))

    value_lists = [
        ["node"],  # type
        ["esp-12e"],  # board
        ["esp8266:esp8266:nodemcuv2"],  # fqbn
        ["wifi"],  # node_communicator
        ["no"],  # actuators->haptic
        ["no"],  # isensors
        ["no"],  # esensors->acceleration_rel
        ["no"],  # esensors->angularvelocity_rel
        ["no"],  # esensors->orientation_abs
        ["no"],  # esensors->glove
        ["no"],  # esensors->shoe
    ]
    all_configs.extend(create_combo(flat_keys, value_lists))

    ####### arduino_nano_33

    value_lists = [
        ["node"],  # type
        ["arduino_nano_33"],  # board
        ["arduino:mbed_nano:nano33ble"],  # fqbn
        ["ble"],  # node_communicator
        ["yes"],  # actuators->haptic
        ["arduino_lsm9ds1"],  # isensors
        ["yes"],  # esensors->acceleration_rel
        ["yes"],  # esensors->angularvelocity_rel
        ["fusion"],  # esensors->orientation_abs
        ["onboard", "serial"],  # esensors->glove
        ["onboard"],  # esensors->shoe
    ]
    all_configs.extend(create_combo(flat_keys, value_lists))

    ####### redbear_duo

    value_lists = [
        ["node"],  # type
        ["redbear_duo"],  # board
        ["RedBear:STM32F2:RedBear_Duo_native" ],
        ["wifi", "ble"],  # node_communicator
        ["yes"],  # actuators->haptic
        ["mpu6050"],  # isensors
        ["yes"],  # esensors->acceleration_rel
        ["yes"],  # esensors->angularvelocity_rel
        ["fusion"],  # esensors->orientation_abs
        ["onboard", "serial"],  # esensors->glove
        ["onboard"],  # esensors->shoe
    ]
    all_configs.extend(create_combo(flat_keys, value_lists))

    value_lists = [
        ["node"],  # type
        ["redbear_duo"],  # board
        ["RedBear:STM32F2:RedBear_Duo_native" ],
        ["wifi", "ble"],  # node_communicator
        ["yes"],  # actuators->haptic
        ["bno055"],  # isensors
        ["yes"],  # esensors->acceleration_rel
        ["yes"],  # esensors->angularvelocity_rel
        ["onboard", "fusion"],  # esensors->orientation_abs
        ["no"],  # esensors->glove
        ["no"],  # esensors->shoe
    ]
    all_configs.extend(create_combo(flat_keys, value_lists))

    value_lists = [
        ["node"],  # type
        ["redbear_duo"],  # board
        ["RedBear:STM32F2:RedBear_Duo_native" ],
        ["wifi", "ble"],  # node_communicator
        ["no"],  # actuators->haptic
        ["no"],  # isensors
        ["no"],  # esensors->acceleration_rel
        ["no"],  # esensors->angularvelocity_rel
        ["no"],  # esensors->orientation_abs
        ["no"],  # esensors->glove
        ["no"],  # esensors->shoe
    ]
    all_configs.extend(create_combo(flat_keys, value_lists))

    ####### mpnrf52840

    value_lists = [
        ["node"],  # type
        ["mpnrf52840"],  # board
        ["adafruit:nrf52:feather52840" ],
        ["ble"],  # node_communicator
        ["yes"],  # actuators->haptic
        ["mpu6050"],  # isensors
        ["yes"],  # esensors->acceleration_rel
        ["yes"],  # esensors->angularvelocity_rel
        ["fusion"],  # esensors->orientation_abs
        ["onboard", "serial"],  # esensors->glove
        ["onboard"],  # esensors->shoe
    ]
    all_configs.extend(create_combo(flat_keys, value_lists))

    value_lists = [
        ["node"],  # type
        ["mpnrf52840"],  # board
        ["adafruit:nrf52:feather52840" ],
        ["ble"],  # node_communicator
        ["yes"],  # actuators->haptic
        ["bno055"],  # isensors
        ["yes"],  # esensors->acceleration_rel
        ["yes"],  # esensors->angularvelocity_rel
        ["onboard", "fusion"],  # esensors->orientation_abs
        ["no"],  # esensors->glove
        ["no"],  # esensors->shoe
    ]
    all_configs.extend(create_combo(flat_keys, value_lists))

    value_lists = [
        ["node"],  # type
        ["mpnrf52840"],  # board
        ["adafruit:nrf52:feather52840" ],
        ["ble"],  # node_communicator
        ["no", "yes"],  # actuators->haptic
        ["no"],  # isensors
        ["no"],  # esensors->acceleration_rel
        ["no"],  # esensors->angularvelocity_rel
        ["no"],  # esensors->orientation_abs
        ["no"],  # esensors->glove
        ["no"],  # esensors->shoe
    ]
    all_configs.extend(create_combo(flat_keys, value_lists))

    return all_configs


def run_test(test_from):
    print("Test build of all valid configs")

    all_configs = generate_all_configs()
    #print(json.dumps(all_configs, indent=4))
    print(f"Number of configs: {len(all_configs)}")

    test_dir = "test_config"
    if os.path.exists(test_dir):
        shutil.rmtree(test_dir)

    counter = 0
    for config_json in all_configs:
        counter =  counter + 1

        if counter < test_from:
            continue
        print(f"Progress: {counter}/{len(all_configs)}")

        os.makedirs(test_dir)
        main_node(test_dir, config_json)
        # This crashes if a compilation error pops out
        compile_arduino_cli(test_dir, config_json)
        shutil.rmtree(test_dir)

    print("Test Finished")


def compile_arduino_cli(test_dir, config_json):

    if config_json["fqbn"] == "TODO":
        print(f"TODO: {config_json}")
        return

    print(f"Building {config_json}")


    # arduino-cli compile  --fqbn  RedBear:STM32F2:RedBear_Duo_native --build-path ./build
    command = f"arduino-cli compile --fqbn {config_json["fqbn"]} --build-path ./build"

    # run() waits for the command to finish
    try:
        result = subprocess.run(
            command.split(" "), 
            cwd=test_dir,
            capture_output=True, 
            text=True, 
            check=True
        )
        print("Success:", result.stdout)
    except subprocess.CalledProcessError as e:
        # Print the specific error message from arduino-cli
        print("--- ARDUINO-CLI ERROR ---")
        print("Exit Code:", e.returncode)
        print("Stdout:", e.stdout)
        print("Stderr:", e.stderr)
        raise


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate a working Bodynodes Sensor project for Arduino"
    )

    # Setup required arguments
    parser.add_argument(
        "--proj-path",
        type=str,
        default=None,
        help="Project folder path with bn_coder_config.json",
    )
    parser.add_argument(
        "--test", action="store_true", help="Test build of all valid configs"
    )

    parser.add_argument(
        "--test-from",
        "-f",
        type=int,
        default=0,
        help="Continue from config number X",
    )

    args = parser.parse_args()

    if args.proj_path:
        main(args.proj_path)
    elif args.test:
        run_test(args.test_from)
