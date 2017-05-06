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


/******************************************************************************/
// vSpinInterface
/******************************************************************************/
bool vSpinInterface::configure(yarp::os::ResourceFinder &rf)
{
    //set the name of the module
    this->moduleName =
            rf.check("name", yarp::os::Value("spinterface")).asString();
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
                         yarp::os::Value(19998),
                         "network input width").asInt();

    std::cout << "Configure Spynnaker Live Spikes Connection:  "
              << moduleName << ", grabber port: " << grabberPortName
              << ", input size: " << width << "," << height
              << ", downsample size: " << downsample_width << "," << downsample_height
              //<< ", local host: " << local_host << ", local port: " << local_port
              << std::endl;

    bool ioSuccess = initSpin(grabberPortName, width, height, downsample_width, downsample_height, flip, (char*) local_host, local_port, absolute_file_path);

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
      //char const* send_event_on_label = "spike_injector_ON_0";
      //char const* send_event_off_label = "spike_injector_OFF_0";

      char const* receive_saliency_label = "LIP_0";

      //SEND_LABELS.push_back((char*)send_event_on_label);
      //SEND_LABELS.push_back((char*)send_event_off_label);
      RECV_LABELS.push_back((char*)receive_saliency_label);

      char* send_labels[SEND_LABELS.size()];
      std::copy(SEND_LABELS.begin(), SEND_LABELS.end(), send_labels);
      char* receive_labels[RECV_LABELS.size()];
      std::copy(RECV_LABELS.begin(), RECV_LABELS.end(), receive_labels);

      this->connection = new SpynnakerLiveSpikesConnection(RECV_LABELS.size(), receive_labels, SEND_LABELS.size(), send_labels, (char*) local_host, local_port);

      // std::string eventOnPortName = "/" + this->moduleName + "/events/on";
      // this->sender_callback_event_on = new SpikeSenderInterface(grabberPortName, eventOnPortName, 0, width, height, d_width, d_height, flip);
      // this->connection->add_start_callback((char *) send_event_on_label, sender_callback_event_on);
      //
      // std::string eventOffPortName = "/" + this->moduleName + "/events/off";
      // this->sender_callback_event_off = new SpikeSenderInterface(grabberPortName, eventOffPortName, 1, width, height, d_width, d_height, flip);
      // this->connection->add_start_callback((char *) send_event_off_label, sender_callback_event_off);

      //add receiver later on
      std::string lipPortName = "/" + this->moduleName + "/salient";
      receiver_callback = new SpikeReceiverInterface(lipPortName, "/lipGazeCtrl/lip:i");
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

//empty line to make gcc happy
