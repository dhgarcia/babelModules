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

#include "spikesPopulation.h"


/******************************************************************************/
// SPIKES_POPULATION
/******************************************************************************/
SpikesPopulation::SpikesPopulation(std::string label, std::string type){
  this->population_label = label;
  this->population_type = type;
}
/******************************************************************************/
void SpikesPopulation::setPopSize(int x, int y)
{
  this->population_size[0]=x;
  this->population_size[1]=y;
}
/******************************************************************************/
std::string SpikesPopulation::getPopLabel()
{
  return this->population_label;
}
/******************************************************************************/
std::string SpikesPopulation::getPopType()
{
  std::cout << population_label << " getting population type: " << population_type << std::endl;
  return this->population_type;
}
/******************************************************************************/


/******************************************************************************/
// SPIKES_RECEIVER_POPULATION
/******************************************************************************/
SpikesReceiverPopulation::SpikesReceiverPopulation(std::string label, std::string type) : SpikesPopulation(label, type) {

}
/******************************************************************************/
bool SpikesReceiverPopulation::setPopulationPort(std::string moduleName, bool broadcast)
{
  //and open the input port
  //if(strictio) this->setStrict();
  //this->strictio = strictio;
  //this->useCallback();

  //yarp::os::BufferedPort< yarp::sig::ImageOf<yarp::sig::PixelBgr> >::open(name + "/img:i");

  if (broadcast) this->spikesPort.open(moduleName + "/" +  this->population_label + "/" + this->population_type + ":o");
  //this->broadcast = broadcast;

  //spikesPort.open(name + "/img/spikes");

  return true;
}
/******************************************************************************/

void SpikesReceiverPopulation::spikesToYarpPort(int time, int n_spikes, int* spikes){

  yarp::os::Bottle& spikeList = this->spikesPort.prepare();

  for (int neuron_id_position = 0;  neuron_id_position < n_spikes; neuron_id_position++)
  {
    std::cout << "Received spike at time" << time << ", from " << this->population_label << " - " << spikes[neuron_id_position] << std::endl;
    spikeList.addInt(spikes[neuron_id_position]);
  }

  //std::cout << "Length of receive spike list is " << spikeList.size() << std::endl;
  this->spikesPort.write();
  spikeList.clear();

}
/******************************************************************************/
void SpikesReceiverPopulation::spikesToSpinnaker(){

}

/******************************************************************************/
// SPIKES_INJECTOR_POPULATION
/******************************************************************************/
SpikesInjectorPopulation::SpikesInjectorPopulation(std::string label, std::string type, int width, int height) : SpikesPopulation(label, type) {

  this->population_size[0]=width;
  this->population_size[1]=height;

}
/******************************************************************************/
void SpikesInjectorPopulation::spikesToYarpPort(int time, int n_spikes, int* spikes){

}
/******************************************************************************/
void SpikesInjectorPopulation::spikesToSpinnaker(){

}
/******************************************************************************/

/******************************************************************************/
// EVENT_SPIKES_INJECTOR_POPULATION
/******************************************************************************/
EventSpikesInjectorPopulation::EventSpikesInjectorPopulation(std::string label, std::string type, int ev_polarity, int ev_width, int ev_height, int pop_width, int pop_height, bool flip) : SpikesInjectorPopulation(label, type, pop_width, pop_height) {
  std::cout << "Event Constructor for " << population_label << std::endl;

  this->flip = flip;

  this->ev_polarity = ev_polarity;
  this->ev_width = ev_width;
  this->ev_height = ev_height;

}
/******************************************************************************/
bool EventSpikesInjectorPopulation::setPopulationPort(std::string moduleName, bool strictio)
{
  return this->open(moduleName, strictio);
}
/******************************************************************************/
bool EventSpikesInjectorPopulation::open(const std::string &name, bool strictio)
{
    //and open the input port
    if(strictio) this->setStrict();
    this->strictio = strictio;
    this->useCallback();

    yarp::os::BufferedPort< ev::vBottle >::open(name + "/" +  this->population_label + "/" + this->population_type + ":i");

    //if (broadcast) viewerPort.open(name + "/img:o");
    //this->broadcast = broadcast;

    spikesPort.open(name + "/" +  this->population_label + "/" + this->population_type + ":o");

    return true;
}

/******************************************************************************/
void EventSpikesInjectorPopulation::close()
{
  spikesPort.close();
  //if (broadcast) spikesPort.close();
  yarp::os::BufferedPort< ev::vBottle >::close();
  //remember to also deallocate any memory allocated by this class
}

/******************************************************************************/
void EventSpikesInjectorPopulation::interrupt()
{
  spikesPort.interrupt();
  //if (broadcast) spikesPort.interrupt();
  yarp::os::BufferedPort< ev::vBottle >::interrupt();
}

/******************************************************************************/
void EventSpikesInjectorPopulation::onRead(ev::vBottle &bot)
{

  // prepare the ports for sending
  yarp::os::Bottle& spikeList = spikesPort.prepare();

  ev::vQueue q = bot.get<ev::AE>();
  for(ev::vQueue::iterator qi = q.begin(); qi != q.end(); qi++)
  {
    auto v = ev::is_event<ev::AddressEvent>(*qi);
    if(v->getChannel()) continue; //TODO add this->channel to select

    if(v->polarity != this->ev_polarity) continue;

    double downsample_factor_width = (double)(this->population_size[0]-1) / (double)(this->ev_width);
    double downsample_factor_height = (double)(this->population_size[1]-1) / (double)(this->ev_height);

    int y = v->y;
    int x = v->x;
    if(this->flip) {
        y = this->ev_width - 1 - y;
        x = this->ev_height - 1 - x;
    }
    int neuronID = (int)(y * downsample_factor_width) * this->population_size[0] + (int)(x * downsample_factor_height);

    spikeList.addInt(neuronID);

    //n_neuron_ids.push_back(neuronID);
    //std::cout << (int)(v->x) << " " << (int)(v->y) << " " << neuronID << std::endl;//<< label << " ID:: " << neuronID << std::endl;
  }

  //connection->send_spikes(label, n_neuron_ids, send_full_keys);
  //n_neuron_ids.clear();

  //send on the processed image
  if(strictio) spikesPort.writeStrict();
  else spikesPort.write();
  spikeList.clear();

}
