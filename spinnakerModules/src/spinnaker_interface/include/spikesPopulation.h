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


#ifndef __SPIKESPOPULATIONS__
#define __SPIKESPOPULATIONS__

//yarp libraries
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

//opencv libraries
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>

//eventdriven libraries
#include <iCub/eventdriven/all.h>
#include <iCub/eventdriven/vtsHelper.h>


class SpikesPopulation : public yarp::os::Thread {

public:
  SpikesPopulation(std::string label, std::string type, bool strict, bool broadcast);

  std::string getPopLabel();
  std::string getPopType();

  void setPopSize(int x, int y);
  void initSpinnakerPopulation(int n_neurons, float run_time_ms, float machine_time_step_ms); // call from spinnakerIOinterface init_population()

  virtual bool setPopulationPorts(std::string moduleName, yarp::os::BufferedPort<yarp::os::Bottle>* readPort)=0;
  virtual void spikesFromSpinnaker(int time, int n_spikes, int* spikes)=0;
  virtual std::vector<int> spikesToSpinnaker()=0;

  void run();
  bool threadInit();

protected:
  bool connectSpikePortToCallbackPort(yarp::os::BufferedPort<yarp::os::Bottle>* readPort);

  std::string population_label;
  std::string population_type;

  int population_size[2];
  int n_neurons;
  float run_time_ms;
  float machine_time_step_ms;

  bool strictio;
  bool broadcast;

  yarp::os::BufferedPort<yarp::os::Bottle> spikesPort;
  //yarp::os::Property population;
  yarp::os::BufferedPort<yarp::os::Bottle> *callbackSpikesPort;

};

class SpikesReceiverPopulation : public SpikesPopulation {

public:
  SpikesReceiverPopulation(std::string label, std::string type, bool strictio=false, bool broadcast=true);

  bool setPopulationPorts(std::string moduleName, yarp::os::BufferedPort<yarp::os::Bottle>* readPort);
  void spikesFromSpinnaker(int time, int n_spikes, int* spikes);
  virtual std::vector<int> spikesToSpinnaker();

private:


};

template <typename T>
class SpikesInjectorPopulation : public SpikesPopulation, public yarp::os::BufferedPort< T >
{

public:
  SpikesInjectorPopulation(std::string label, std::string type, int width, int height, std::string source, bool strictio=true, bool broadcast=false);

  virtual bool setPopulationPorts(std::string moduleName, yarp::os::BufferedPort<yarp::os::Bottle>* readPort);
  virtual void spikesFromSpinnaker(int time, int n_spikes, int* spikes);
  std::vector<int> spikesToSpinnaker();

  bool open(const std::string &name, bool strictio, bool broadcast);

protected:
  std::string sourcePortName;
  yarp::os::BufferedPort< T > viewerPort;

  int times_onRead=0;

private:

};

class EventSpikesInjectorPopulation : public SpikesInjectorPopulation< ev::vBottle > {

public:
  EventSpikesInjectorPopulation(std::string label, std::string type, int ev_polarity, int ev_width, int ev_height, int pop_width, int pop_height, bool flip, std::string source="/zynqGrabber/vBottle:o", bool strictio=true, bool broadcast=false);

  //bool    open(const std::string &name, bool strictio, bool broadcast=false);
  void    close();
  void    interrupt();
  //this is the entry point to your main functionality
  void    onRead(ev::vBottle &bot);

private:
  bool flip;

  int ev_polarity;
  int ev_width;
  int ev_height;


};


class VisionSpikesInjectorPopulation : public SpikesInjectorPopulation < yarp::sig::ImageOf<yarp::sig::PixelBgr> > {

public:
  VisionSpikesInjectorPopulation(std::string label, std::string type, int v_width, int v_height, int pop_width, int pop_height, std::string source="/icub/cam/left", bool strictio=true, bool broadcast=false);

  //bool    open(const std::string &name, bool strictio, bool broadcast=true);
  void    close();
  void    interrupt();
  //this is the entry point to your main functionality
  void    onRead(yarp::sig::ImageOf<yarp::sig::PixelBgr> &bot);

private:
  //bool flip;

  //int v_polarity;
  int v_width;
  int v_height;

  // Create a 2D vector to map x,y pos to a neuron ID
  std::vector<std::vector<int> > createIDMap(int height, int width);
  std::vector<std::vector<int> > InputIDMap;

};

class AudioSpikesInjectorPopulation : public SpikesInjectorPopulation < yarp::os::Bottle > {

public:
  AudioSpikesInjectorPopulation(std::string label, std::string type, int pop_width, int pop_height, std::string source="/icub/speach", bool strictio=true, bool broadcast=false);

  //bool    open(const std::string &name, bool strictio, bool broadcast=false);
  void    close();
  void    interrupt();
  //this is the entry point to your main functionality
  void    onRead(yarp::os::Bottle &bot);

private:


};



#endif /* __SPIKESPOPULATIONS__ */
