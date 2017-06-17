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


class SpikesPopulation {

public:
  SpikesPopulation(std::string label, std::string type);
  virtual ~SpikesPopulation(){};

  std::string getPopLabel();
  std::string getPopType();
  void setPopSize(int x, int y);


  virtual bool setPopulationPort(std::string moduleName, bool broadcast){}//=0;
  virtual void spikesToYarpPort(int time, int n_spikes, int* spikes){}//=0;
  virtual void spikesToSpinnaker(){}//=0;

protected:
  int population_size[2];

  std::string population_label;
  std::string population_type;

  yarp::os::BufferedPort<yarp::os::Bottle> spikesPort;

};

class SpikesReceiverPopulation : public SpikesPopulation {

public:
  SpikesReceiverPopulation(std::string label, std::string type);

  void spikesToYarpPort(int time, int n_spikes, int* spikes);
  void spikesToSpinnaker();
  bool setPopulationPort(std::string moduleName, bool broadcast=true);

private:


};

class SpikesInjectorPopulation : public SpikesPopulation {

public:
  SpikesInjectorPopulation(std::string label, std::string type, int width, int height);
  virtual ~SpikesInjectorPopulation(){};

  virtual void spikesToYarpPort(int time, int n_spikes, int* spikes);
  virtual void spikesToSpinnaker();

private:


};

class EventSpikesInjectorPopulation : public SpikesInjectorPopulation, public yarp::os::BufferedPort< ev::vBottle > {

public:
  EventSpikesInjectorPopulation(std::string label, std::string type, int ev_polarity, int ev_width, int ev_height, int pop_width, int pop_height, bool flip);

  bool setPopulationPort(std::string moduleName, bool strictio=true);
  //virtual void spikesToYarpPort(int time, int n_spikes, int* spikes);
  //void spikesToSpinnaker();

  bool    open(const std::string &name, bool strictio);
  void    close();
  void    interrupt();
  //this is the entry point to your main functionality
  void    onRead(ev::vBottle &bot);

private:

  bool flip;
  bool strictio;

  int ev_polarity;
  int ev_width;
  int ev_height;

};




#endif /* __SPIKESPOPULATIONS__ */
