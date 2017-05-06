/*
 * Copyright (C) 2017 CRNS
 * Author: Daniel Hernandez Garcia
 * email:  daniel.hernandez@plymouth.ac.uk
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 * *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */


#ifndef __ICUB_SPINNAKERVISUALIZER__
#define __ICUB_SPINNAKERVISUALIZER__

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

//#include <SpiNNakerIO/SpynnakerLiveSpikesConnection.h>
#include <SpynnakerLiveSpikesConnection.h>

#include "raster_view/RasterPlot.h"
#include "utilities/ColourReader.h"

class visualizerSpinnInterface : public yarp::os::RFModule
{

    RasterPlot *plotter;
    ColourReader *colour_reader;

    SpynnakerLiveSpikesConnection *connection;// =

    bool initSpin(std::string grabberPortName, int width, int height, int d_width, int d_height, bool flip, char *local_host, int local_port, char* absolute_file_path);

    bool initSpin(std::string grabberPortName, int d_width, int d_height, bool broadcast);


public:

    //the virtual functions that need to be overloaded
    virtual bool configure(yarp::os::ResourceFinder &rf);
    virtual bool interruptModule();
    virtual bool close();
    virtual double getPeriod();
    virtual bool updateModule();

    std::string moduleName;

};


#endif
//empty line to make gcc happy
