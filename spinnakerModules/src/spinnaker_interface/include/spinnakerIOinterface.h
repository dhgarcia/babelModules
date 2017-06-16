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


#ifndef __YARP_SPINNAKERIOINTERFACE__
#define __YARP_SPINNAKERIOINTERFACE__

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

//opencv libraries
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>

//#include <SpiNNakerIO/SpynnakerLiveSpikesConnection.h>
#include <SpynnakerLiveSpikesConnection.h>


#include <vector>
#include <iterator>

#include "spikesPopulation.h"

std::vector<std::vector<int> > createIDMap(int height, int width);



class SpikeSenderInterface : public SpikesStartCallbackInterface{
public:
    SpikeSenderInterface(std::string imgPortName, std::string spPortName, int width, int height, int d_width, int d_height, bool isFlip);

    //void init_population(char *label, int n_neurons, float run_time_ms, float machine_time_step_ms);
    void spikes_start(char *label, SpynnakerLiveSpikesConnection *connection);


private:

    std::string spikePortName;
    std::string imagePortName;

    int source_width;
    int source_height;
    int downsample_width;
    int downsample_height;
    //int polarity;
    bool flip;

    std::vector<std::vector<int> > InputIDMap;

};

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

//For when not using spinnaker //this should be remove and put on its own module
class visionSpikeSender: public yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelBgr> >
{

private:

  bool strictio;
  bool broadcast;

  int downsample_width;
  int downsample_height;

  std::vector<std::vector<int> > InputIDMap;

  yarp::sig::ImageOf<yarp::sig::PixelBgr> * outImage;
  /*//! the list of output images
  std::vector<yarp::sig::ImageOf<yarp::sig::PixelBgr> *> outImage;*/

  yarp::os::BufferedPort< yarp::sig::ImageOf<yarp::sig::PixelBgr> > viewerPort;
  yarp::os::BufferedPort<yarp::os::Bottle> outPort;
  /*//! the list of output ports for images
  std::vector<yarp::os::BufferedPort<
      yarp::sig::ImageOf<yarp::sig::PixelBgr> > *> outports;*/

public:

  visionSpikeSender(int width, int height);

  bool    open(const std::string &name, bool strictio, bool broadcast);
  void    close();
  void    interrupt();
  //this is the entry point to your main functionality
  void    onRead(yarp::sig::ImageOf<yarp::sig::PixelBgr> &frame);

};


// class SpikesPopulation
// { //this will change completely
//
// public:
//   SpikesPopulation(std::string label, std::string type);
//
//   std::string getPopLabel();
//   std::string getPopType();
//
//   void setPopSize(int x, int y);
//
// private:
//   //std::map<std::string, std::string> populations;
//   std::string label;
//   std::string type;
//   int population_size[2];
//
//   yarp::os::BufferedPort<yarp::os::Bottle> spikesPort; //this maybe inheritance of the hold class?
//   yarp::os::Property population; //this maybe inheritance of the hold class?
//
// };

class SpikesCallbackInterface : public SpikeReceiveCallbackInterface, public SpikesStartCallbackInterface, public SpikeInitializeCallbackInterface
{

public:
  SpikesCallbackInterface(std::string name, std::map<std::string, SpikesPopulation*> &spikes, bool wait_for_start=true);
  virtual void init_population(char *label, int n_neurons, float run_time_ms,
                              float machine_time_step_ms);
  virtual void spikes_start(char *label,
                            SpynnakerLiveSpikesConnection *connection);
  virtual void receive_spikes(char *label, int time, int n_spikes, int* spikes);

private:
  std::string spinInterfaceName;

  bool ready_to_start;
  bool database_read;
  bool simulation_started;

  int n_populations_to_read;

  pthread_mutex_t start_mutex;
  pthread_cond_t start_condition;

  std::map<std::string, SpikesPopulation*> spikes_structure;


};

class SpinnakerInterface : public yarp::os::RFModule
{

  //std::vector<SpikesPopulation> spikes;

  SpikesCallbackInterface *spikes_interface;

  SpynnakerLiveSpikesConnection *connection;// =



    SpikeSenderInterface      *sender_callback;
    SpikeReceiverInterface    *receiver_callback;


    visionSpikeSender  *sender_yarp_spikes;


    bool initialise(std::string spinnName, std::map<std::string, SpikesPopulation*> &spikes, bool wait, char *local_host, int local_port, char* absolute_file_path);


public:

  //the virtual functions that need to be overloaded
  virtual bool configure(yarp::os::ResourceFinder &rf); //configure all the module parameters and return true if successful
  virtual bool interruptModule();                       //interrupt the ports
  virtual bool close();                                 //close and shut down
  virtual bool respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply); //rpc port
  virtual double getPeriod();
  virtual bool updateModule();

  std::string moduleName;


};


#endif
//empty line to make gcc happy
