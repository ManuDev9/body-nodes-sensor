# body-nodes-coder
This project is for a script that can easily create Bodynodes projects for development environment.

At the moment these are the platform the project supports.
- Arduino

For example if you want to create your own hardware sensors and you have your own platform at home.
You can find out more about the hardware and logical requirements in here: https://github.com/ManuDev9/body-nodes-specs
Specifically, here is what hardware sensors you can connect to the board the https://github.com/ManuDev9/body-nodes-specs/blob/master/SensorTypes.spec

Note: the board is supposed to be able to connect via WiFi to a network and be able to use Multicast on the Wifi (https://en.wikipedia.org/wiki/Multicast)

You just need to setup the files:
- Arduino:
  - bn_coder_config.json
    - Before running the coder
    - It should be inside the main project directory
    - Look inside the example folder to get an idea
    - Set "board" : "custom" to make use of a custom board
  - BnNodeSpecific.h, BnNodeSpecific.cpp
    - It gets created afterwards
    - It will need to change if you have a custom board or custom pins setup
  - Example:

        python3 BnPythonNodesCoderArduino.py examples/

    - And the directory examples will be populated by files


Use the command "python3 BnPythonNodesCoderArduino.py h" to print the help and check how to run it

Have a look at the files you need and adapt them for your project in order to create your Bodynodes.

