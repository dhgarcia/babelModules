iCubBabelModules
===========


This repository is for storing useful iCub modules for the SpiNNaker Visual Attention and Naming experiments for BABEL.

## Structure

new modules should be added under the "src/" folder, in a folder of their own (e.g.: src/myModuleFolder/src/module.cpp)

xml files are stored as templates on the /app/scripts/ folder (e.g.: /app/scripts/myModule.xml.template). This is so people can adapt the scripts to their own setups/machines without conflicting xml files.

## Dependencies

- [YARP](https://github.com/robotology/yarp)
- [icub-contrib-common](https://github.com/robotology/icub-contrib-common)

## Installation

modules are installed from sources (!!!!DON'T COMMIT BINARIES!!!!).

execute the following commands to install the modules:

    git clone https://github.com/CRNS-CognitiveRoboticsLab/iCubBabelModules.git
    cd iCubBabelModules && mkdir build && cd build && cmake ..

after generating, you can make and install. If you want to install to the system bin/ directory, you will need sudo - otherwise, specify the installation path on ccmake.

    make && sudo make install
