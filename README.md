
# FIRAClient
> Client base code for the VSSS FiraSim simulator based on RoboCin FIRAClient and ported to Python 

This repo was forked from https://github.com/VSSSLeague/FIRAClient

## Description
This repository was created for the purpose of using the protobuf comunication protocol on Pytohn.

This project is capable of:
- Receive field, referee and robot information from FiraSim on Python
- Send commands to FiraSim Simulator, such as robot controls and positions from Python.

## Dependencies
- This set of clients were made to run on Linux
- The build-essential package (`$ sudo apt install build-essential`)
- [protobuf](https://github.com/google/protobuf)
- Python3
- Qt5

    You can install this dependencies with:
    ```sh
    $ sudo apt install -qq build-essential qtdeclarative5-dev libeigen3-dev protobuf-compiler libprotobuf-dev libdc1394-22 libdc1394-22-dev cmake libv4l-0 libopencv-dev freeglut3-dev python3
    ```
    
*Tested with: Qt 5.12.8, protoc 3.x, Ubuntu 20.04 LTS. and Ubuntu based Linux (Mint 20.01, Pop!_OS 20.04 LTS)*

## Simulation

FIRAClient was built to communicate with some specific programs:

- [FIRASim](https://github.com/robocin/FIRASim): The simulator containing the field and robot environment.
- [VSSSReferee](https://github.com/VSSSLeague/VSSReferee): A auxiliary program to detect goals, fouls and deal with game interruptions.
 
You can install this programs following the instructions on [SETUP.md](https://github.com/YapiraUFPR/FIRAClient/blob/main/SETUP.md)

## Compile

 - First clone this repository:

    ```sh
    git clone https://github.com/YapiraUFPR/FIRAClient.git
    cd FIRAClient
    ```

 - Run the command bellow to generate and compile the clients and the shared object that will be used on the bridge:

    ```sh
    mkdir build
    cd build/
    qmake ..
    make
    ```
    
    This will generate the `libfira.so` file that the bridge file uses to comunicate withe the C++ FIRAClient base

    And then you can test run:

    ```sh
    cd ..
    python3 bridge.py
    ```

    This sould play a message testing the lib.

    Use `python3 bridge.py --help` to see documentation

    This bridge file will be used on the rest of the pytohn program. main.

    Now you can run the code (make sure you are running both FIRASim and VSSReferee)

    ```sh
    ./main.py
    ```
## Usage

You can run the code by looking at the main.py example and the bridge.py docstrings

### Contributors: 
- [Artur Coelho](https://github.com/arturtcoelho) 
- Gabriel Hishida
- Allan Cedric
