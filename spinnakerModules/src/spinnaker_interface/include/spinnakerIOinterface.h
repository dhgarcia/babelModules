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

#include <SpynnakerLiveSpikesConnection.h>

//#include <vector>
//#include <iterator>
//#include <map>
//#include <list>
//#include <math.h>
#include <cstring>

#include "spikesPopulation.h"


class SpikesCallbackInterface : public SpikeReceiveCallbackInterface, public SpikesStartCallbackInterface, public SpikeInitializeCallbackInterface
{

public:
  SpikesCallbackInterface(std::string name, std::map<std::string, SpikesPopulation*> &spikes, bool wait_for_start=true);
  virtual void init_population(char *label, int n_neurons, float run_time_ms,
                              float machine_time_step_ms);
  virtual void spikes_start(char *label,
                            SpynnakerLiveSpikesConnection *connection);
  virtual void receive_spikes(char *label, int time, int n_spikes, int* spikes);

  void startSpikesInterface();

private:
  std::string spinInterfaceName;

  bool ready_to_start;
  bool database_read;
  bool simulation_started;

  int n_populations_to_read;

  pthread_mutex_t start_mutex;
  pthread_cond_t start_condition;
  pthread_mutex_t point_mutex;

  std::map<std::string, SpikesPopulation*> spikes_structure;
  std::vector<yarp::os::BufferedPort<yarp::os::Bottle >* > readPorts;

};

class SpinnakerInterface : public yarp::os::RFModule
{

  std::map<std::string, SpikesPopulation*> spikes;
  SpikesCallbackInterface *spikes_interface;
  SpynnakerLiveSpikesConnection *connection;// =

  bool initialise(std::string spinnName, bool wait, char *local_host, int local_port, char* absolute_file_path);

public:

  //the virtual functions that need to be overloaded
  virtual bool configure(yarp::os::ResourceFinder &rf); //configure all the module parameters and return true if successful
  virtual bool interruptModule();                       //interrupt the ports
  virtual bool close();                                 //close and shut down
  virtual bool respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply); //rpc port
  virtual double getPeriod();
  virtual bool updateModule();

  std::string moduleName;
  yarp::os::Port handlePort;

};


#endif
//empty line to make gcc happy
