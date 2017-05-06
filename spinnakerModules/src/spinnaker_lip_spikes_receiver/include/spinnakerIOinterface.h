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


#ifndef __ICUB_SPINNAKERIOINTERFACE__
#define __ICUB_SPINNAKERIOINTERFACE__

#include <yarp/os/all.h>

//#include <SpiNNakerIO/SpynnakerLiveSpikesConnection.h>
#include <SpynnakerLiveSpikesConnection.h>


// class SpikeSenderInterface : public SpikesStartCallbackInterface{
// public:
//     SpikeSenderInterface(std::string evPortName, std::string spPortName, int desired_polarity, int width, int height, int d_width, int d_height, bool isFlip);
//
//     //void init_population(char *label, int n_neurons, float run_time_ms, float machine_time_step_ms);
//     void spikes_start(char *label, SpynnakerLiveSpikesConnection *connection);
//
// private:
//
//     std::string spikePortName;
//     std::string eventPortName;
//
//     int source_width;
//     int source_height;
//     int downsample_width;
//     int downsample_height;
//     int polarity;
//     bool flip;
//
//     //for helping with timestamp wrap around
//     ev::vtsHelper unwrapper;
// };

class SpikeReceiverInterface : public SpikeReceiveCallbackInterface{
public:
    SpikeReceiverInterface(std::string spPortName, std::string PortName);

    void receive_spikes(char *label, int time, int n_spikes, int* spikes);


private:
    pthread_mutex_t *cond;
    std::string portName;
    std::string portSpikesReceive;
    yarp::os::BufferedPort<yarp::os::Bottle> spikesPort;

};



class vSpinInterface : public yarp::os::RFModule
{


    //SpikeSenderInterface      *sender_callback_event_on;
    //SpikeSenderInterface      *sender_callback_event_off;
    SpikeReceiverInterface    *receiver_callback;

    SpynnakerLiveSpikesConnection *connection;// =

    bool initSpin(std::string grabberPortName, int width, int height, int d_width, int d_height, bool flip, char *local_host, int local_port, char* absolute_file_path);


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
