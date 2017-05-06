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

#include "spinnakerIOinterface.h"
#include <map>
#include <list>
#include <math.h>

using namespace ev;

/******************************************************************************/
// vSpinInterface
/******************************************************************************/
bool vSpinInterface::configure(yarp::os::ResourceFinder &rf)
{
    //set the name of the module
    this->moduleName =
            rf.check("name", yarp::os::Value("spinnakerIO")).asString();
    setName(moduleName.c_str());

    std::string grabberPortName =
            rf.check("grabber", yarp::os::Value("/zynqGrabber/vBottle:o")).asString();

    int height =
            rf.check("height", yarp::os::Value(128)).asInt();
    int width =
            rf.check("width", yarp::os::Value(128)).asInt();

    int downsample_height = rf.check("downsample_height",
                          yarp::os::Value(32),
                          "network input height").asInt();
    int downsample_width = rf.check("downsample_width",
                         yarp::os::Value(32),
                         "network input width").asInt();


    bool flip = rf.check("flip");

    char const* local_host = NULL;
    char* absolute_file_path = NULL;
    int local_port = rf.check("hand_shake_port",
                         yarp::os::Value(19999),
                         "network input width").asInt();

    std::cout << "Configure Spynnaker Live Spikes Connection:  "
              << moduleName << ", grabber port: " << grabberPortName
              << ", input size: " << width << "," << height
              << ", downsample size: " << downsample_width << "," << downsample_height
              //<< ", local host: " << local_host << ", local port: " << local_port
              << std::endl;

    //bool ioSuccess = initSpin(grabberPortName, width, height, downsample_width, downsample_height, flip, (char*) local_host, local_port, absolute_file_path);

    bool spinnaker = rf.check("spinnaker") &&
            rf.check("spinnaker", yarp::os::Value(true)).asBool();;
    bool ioSuccess = false;

    if (spinnaker) {
      ioSuccess = initSpin(grabberPortName, width, height, downsample_width, downsample_height, flip, (char*) local_host, local_port, absolute_file_path);
    } else {
      ioSuccess = initSpin(grabberPortName, width, height, downsample_width, downsample_height, true);
    }

    return ioSuccess;

}

/******************************************************************************/
bool vSpinInterface::initSpin(std::string grabberPortName, int width, int height, int d_width, int d_height, bool flip, char *local_host, int local_port, char* absolute_file_path)
{

    bool success = false;

    try {

      std::vector<char*> SEND_LABELS;
      std::vector<char*> RECV_LABELS;

      //labels for spike injection, must be the same name on the other side, labels go in the SPYNNAKER_DATABASE
      char const* send_event_on_label = "spike_injector_ON_0";
      char const* send_event_off_label = "spike_injector_OFF_0";

      char const* receive_saliency_label = "LIP_0";

      SEND_LABELS.push_back((char*)send_event_on_label);
      SEND_LABELS.push_back((char*)send_event_off_label);
      RECV_LABELS.push_back((char*)receive_saliency_label);

      char* send_labels[SEND_LABELS.size()];
      std::copy(SEND_LABELS.begin(), SEND_LABELS.end(), send_labels);
      char* receive_labels[RECV_LABELS.size()];
      std::copy(RECV_LABELS.begin(), RECV_LABELS.end(), receive_labels);

      this->connection = new SpynnakerLiveSpikesConnection(RECV_LABELS.size(), receive_labels, SEND_LABELS.size(), send_labels, (char*) local_host, local_port);

      std::string eventOnPortName = "/" + this->moduleName + "/events/on";
      this->sender_callback_event_on = new SpikeSenderInterface(grabberPortName, eventOnPortName, 0, width, height, d_width, d_height, flip);
      this->connection->add_start_callback((char *) send_event_on_label, sender_callback_event_on);

      std::string eventOffPortName = "/" + this->moduleName + "/events/off";
      this->sender_callback_event_off = new SpikeSenderInterface(grabberPortName, eventOffPortName, 1, width, height, d_width, d_height, flip);
      this->connection->add_start_callback((char *) send_event_off_label, sender_callback_event_off);

      //add receiver later on
      std::string lipPortName = "/" + this->moduleName + "/lip/salient";
      std::string salientPortName = "/spinnakerCtrl/gaze:i";
      this->receiver_callback = new SpikeReceiverInterface(lipPortName, salientPortName);
      this->connection->add_receive_callback((char*) receive_saliency_label, receiver_callback);

      success = true;
      std::cout << "Spynnaker Live Spikes Connection set. \n Waiting for database to be ready... " << std::endl;

    } catch (char const* msg){
      printf("%s \n", msg);
      success = false;
    }

    return success;
}
/******************************************************************************/
bool vSpinInterface::initSpin(std::string grabberPortName, int width, int height, int d_width, int d_height, bool flip)
{
  std::string spikesPortName = "/" + this->moduleName;

  this->sender_on_yarp_spikes = new eventSpikeSender(width, height, d_width, d_height, 0, flip);
  this->sender_on_yarp_spikes->open(spikesPortName + "/events/on", true);

  this->sender_off_yarp_spikes = new eventSpikeSender(width, height, d_width, d_height, 1, flip);
  this->sender_off_yarp_spikes->open(spikesPortName + "/events/off", true);

  return true;
}


/******************************************************************************/
bool vSpinInterface::interruptModule()
{
    //outputManager.stop();
    //inputManager.interrupt();
    yarp::os::RFModule::interruptModule();
    return true;
}

/******************************************************************************/
bool vSpinInterface::close()
{
    //outputManager.threadRelease();
    //inputManager.close();
    yarp::os::RFModule::close();
    return true;
}

/******************************************************************************/
bool vSpinInterface::updateModule()
{
    return true;
}

/******************************************************************************/
double vSpinInterface::getPeriod()
{
    return 1;
}



/******************************************************************************/
// SPINNAKER_IO - SENDER
/******************************************************************************/

SpikeSenderInterface::SpikeSenderInterface(std::string evPortName, std::string spPortName, int desired_polarity, int width, int height, int d_width, int d_height, bool isFlip){
  this->eventPortName = evPortName;
  this->spikePortName = spPortName;

  //here we should initialise the module
  this->source_height = height; //240;//128; //256;
  this->source_width = width; //304;//128; //256;
  this->downsample_width = d_width;
  this->downsample_height = d_height;
  this->polarity = desired_polarity;

  this->flip = isFlip;
}

void SpikeSenderInterface::spikes_start(char *label, SpynnakerLiveSpikesConnection *connection){
  std::vector<int> n_neuron_ids; //send_spikes parameter
  bool send_full_keys=false; //send_spikes parameter

  yarp::os::BufferedPort<vBottle> spikesPort;
  spikesPort.open(spikePortName.c_str()); //get address parameter
  yarp::os::Network::connect(this->eventPortName.c_str(), this->spikePortName.c_str());

  while (true) {
    //read the port
    vBottle *bot = NULL; // yarp::os::Bottle *spikes = NULL;
    bot = spikesPort.read(); //??

    if (bot!=NULL) { // check we actually got something
      //create event queue
      vQueue q = bot->get<AE>();
      for(vQueue::iterator qi = q.begin(); qi != q.end(); qi++)
      {
        auto v = is_event<AddressEvent>(*qi);
        if(v->getChannel()) continue; //TODO add this->channel to select

        if(v->polarity != this->polarity) continue;

        double downsample_factor_width = (double)(this->downsample_width-1) / (double)(this->source_width);
        double downsample_factor_height = (double)(this->downsample_height-1) / (double)(this->source_height);

        int y = v->y;
        int x = v->x;
        if(this->flip) {
            y = this->source_width - 1 - y;
            x = this->source_height - 1 - x;
        }
        int neuronID = (int)(y * downsample_factor_width) * this->downsample_width + (int)(x * downsample_factor_height);

        n_neuron_ids.push_back(neuronID);
        //std::cout << (int)(v->x) << " " << (int)(v->y) << " " << neuronID << std::endl;//<< label << " ID:: " << neuronID << std::endl;
      }

      connection->send_spikes(label, n_neuron_ids, send_full_keys);
      n_neuron_ids.clear();
    }

  } //while loop


}



/******************************************************************************/
// SPINNAKER_IO - RECEIVER
/******************************************************************************/

SpikeReceiverInterface::SpikeReceiverInterface(std::string spPortName, std::string PortName){
  this->portSpikesReceive = spPortName;
  this->portName = PortName;

  this->spikesPort.open(this->portSpikesReceive.c_str()); //get address parameter
  yarp::os::Network::connect(this->portSpikesReceive.c_str(), this->portName.c_str());
}

void SpikeReceiverInterface::receive_spikes(char *label, int time, int n_spikes, int* spikes){

    yarp::os::Bottle& spikeList = this->spikesPort.prepare();

    for (int neuron_id_position = 0;  neuron_id_position < n_spikes; neuron_id_position++)
    {
      std::cout << "Received spike at time" << time << ", from " << label << " - " << spikes[neuron_id_position] << std::endl;
      spikeList.addInt(spikes[neuron_id_position]);
    }

    //std::cout << "Length of receive spike list is " << spikeList.size() << std::endl;
    this->spikesPort.write();
    spikeList.clear();
}


/******************************************************************************/
// EVENT YARP STUFF (NON SPINNAKER)
/******************************************************************************/

eventSpikeSender::eventSpikeSender(int width, int height, int d_width, int d_height, int polarity, bool flip){
  //InputIDMap = createIDMap(height, width);
  this->downsample_width = d_width;
  this->downsample_height = d_height;
  this->source_width = width;
  this->source_height = height;
  this->flip = flip;
  this->polarity = polarity;
}

/******************************************************************************/
bool eventSpikeSender::open(const std::string &name, bool strictio)
{
    //and open the input port
    if(strictio) this->setStrict();
    this->strictio = strictio;
    this->useCallback();

    yarp::os::BufferedPort< ev::vBottle >::open(name);

    //if (broadcast) viewerPort.open(name + "/img:o");
    //this->broadcast = broadcast;

    outPort.open(name + ":o");

    return true;
}

/******************************************************************************/
void eventSpikeSender::close()
{
  outPort.close();
  //if (broadcast) viewerPort.close();
  yarp::os::BufferedPort< ev::vBottle >::close();
  //remember to also deallocate any memory allocated by this class
}

/******************************************************************************/
void eventSpikeSender::interrupt()
{
  outPort.interrupt();
  //if (broadcast) viewerPort.interrupt();
  yarp::os::BufferedPort< ev::vBottle >::interrupt();
}

/******************************************************************************/
void eventSpikeSender::onRead(ev::vBottle &bot)
{

  // prepare the ports for sending
  yarp::os::Bottle& spikeList = outPort.prepare();

  ev::vQueue q = bot.get<ev::AE>();
  for(ev::vQueue::iterator qi = q.begin(); qi != q.end(); qi++)
  {
    auto v = ev::is_event<ev::AddressEvent>(*qi);
    if(v->getChannel()) continue; //TODO add this->channel to select

    if(v->polarity != this->polarity) continue;

    double downsample_factor_width = (double)(this->downsample_width-1) / (double)(this->source_width);
    double downsample_factor_height = (double)(this->downsample_height-1) / (double)(this->source_height);

    int y = v->y;
    int x = v->x;
    if(this->flip) {
        y = this->source_width - 1 - y;
        x = this->source_height - 1 - x;
    }
    int neuronID = (int)(y * downsample_factor_width) * this->downsample_width + (int)(x * downsample_factor_height);

    spikeList.addInt(neuronID);

    //n_neuron_ids.push_back(neuronID);
    //std::cout << (int)(v->x) << " " << (int)(v->y) << " " << neuronID << std::endl;//<< label << " ID:: " << neuronID << std::endl;
  }

  //connection->send_spikes(label, n_neuron_ids, send_full_keys);
  //n_neuron_ids.clear();

  //send on the processed image
  if(strictio) outPort.writeStrict();
  else outPort.write();
  spikeList.clear();

}

//empty line to make gcc happy
