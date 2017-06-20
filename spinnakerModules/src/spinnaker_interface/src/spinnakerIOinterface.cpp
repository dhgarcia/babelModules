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


/******************************************************************************/
// SpinnakerInterface
/******************************************************************************/
bool SpinnakerInterface::configure(yarp::os::ResourceFinder &rf)
{
    //set the name of the module
    moduleName = rf.check("name", yarp::os::Value("/spinnakerIO")).asString();
    setName(moduleName.c_str());

    if ( !handlePort.open(getName()) ) {
      std::cout << getName() << ":unale to open port" << '\n';
      return false;
    }
    attach(handlePort);


    bool wait = rf.check("wait_for_start") &&
            rf.check("wait_for_start", yarp::os::Value(true)).asBool();


    std::map<std::string, SpikesPopulation*> spikes;

    //set the spikes streamers
    yarp::os::Bottle * spikesList = rf.find("spikes").asList();
    if(!spikesList) {
      //spikesList = &tempSpikesList;  //comment for now what would the default spikes supposed to be?
      std::cerr << "Error: spikes are not provided " << std::endl;
      return false;
    }
    std::cout << spikesList->toString() << std::endl;

    if(spikesList->size() % 3) {
        std::cerr << "Error: spikes incorrectly configured " << std::endl;
        return false;
    }

    int nSpikes = spikesList->size() / 3;
    std::cout << "# of Spike Populations " << nSpikes << std::endl;

    //for each spikes open a SpikesPopulation

    for(int i = 0; i < nSpikes; i++) {

      SpikesPopulation * newPopulation;

      //extract the population label
      std::string label_name = spikesList->get(i*3).asString();
      std::string population_type = spikesList->get(i*3+1).asString();
      yarp::os::Bottle * populaion_type_list = spikesList->get(i*3 + 2).asList();

      if (population_type == "receiver") {
        newPopulation = new SpikesReceiverPopulation(label_name, population_type);
      } else if (population_type == "event_injector") {
        int ev_polarity = populaion_type_list->get(0).asInt();
        int ev_width = populaion_type_list->get(1).asInt();
        int ev_height = populaion_type_list->get(2).asInt();
        int pop_width = populaion_type_list->get(3).asInt();
        int pop_height = populaion_type_list->get(4).asInt();
        bool flip = populaion_type_list->get(5).asBool();
        newPopulation = new EventSpikesInjectorPopulation(label_name, "injector", ev_polarity, ev_width, ev_height, pop_width, pop_height, flip);
      } else if (population_type == "vision_injector") {
        int img_width = populaion_type_list->get(0).asInt();
        int img_height = populaion_type_list->get(1).asInt();
        int pop_width = populaion_type_list->get(2).asInt();
        int pop_height = populaion_type_list->get(3).asInt();
        newPopulation = new VisionSpikesInjectorPopulation(label_name, "injector", img_width, img_height, pop_width, pop_height);
      } else if (population_type == "audio_injector") {
        int x = populaion_type_list->get(0).asInt();
        int y = populaion_type_list->get(1).asInt();
        newPopulation = new AudioSpikesInjectorPopulation(label_name, "injector", x, y);
      } else {
        newPopulation = NULL; //new SpikesPopulation(label_name, population_type); //abstract cannot be instantiated
      }

      //spikes.push_back(newPopulation);
      spikes[label_name] = newPopulation;
    }


    char const* local_host = NULL;
    char* absolute_file_path = NULL;
    int local_port = rf.check("hand_shake_port",
                         yarp::os::Value(19999),
                         "network input width").asInt();

    std::cout << "Configure Spynnaker Live Spikes Connection:  "
              << moduleName
              //<< ", grabber port: " << grabberPortName
              //<< ", input size: " << width << "," << height
              //<< ", downsample size: " << downsample_width << "," << downsample_height
              //<< ", local host: " << local_host << ", local port: " << local_port
              << std::endl;

    bool ioSuccess = false;

    ioSuccess = initialise(moduleName, spikes, wait, (char*) local_host, local_port, absolute_file_path);

    return ioSuccess;
}

/******************************************************************************/
bool SpinnakerInterface::initialise(std::string spinnName, std::map<std::string, SpikesPopulation*> &spikes, bool wait, char *local_host, int local_port, char* absolute_file_path)
{

    bool success = false;
    std::cout << "Spynnaker Interface ... Initialising " << std::endl;

    try {

      std::vector<char*> SEND_LABELS;
      std::vector<char*> RECV_LABELS;
      std::cout << "Spynnaker Interface ... set labels " << std::endl;

      for (std::map<std::string, SpikesPopulation*>::iterator it = spikes.begin(); it!=spikes.end(); ++it) {

        std::string label_name(it->second->getPopLabel());
        char *pop_label = new char[label_name.length() + 1];
        std::strcpy(pop_label, label_name.c_str());

        if(strcmp(it->second->getPopType().c_str(),"injector")==0){
          //char const* label = "LIP_0";
          //SEND_LABELS.push_back((char*)it->second->getPopLabel().c_str());
          SEND_LABELS.push_back(pop_label);
          //SEND_LABELS.push_back((*labels)[i]);
          //std::cout << "LABELs: " << SEND_LABELS.back() << std::endl;
        }
        if(strcmp(it->second->getPopType().c_str(),"receiver")==0){
          RECV_LABELS.push_back(pop_label);
          //std::cout << "LABELs: " << RECV_LABELS.back() <<  std::endl;
        }
        //std::cout << "LABEL: " << (char*)it->second->getPopLabel().c_str() << " " << (char*)it->second->getPopType().c_str() << std::endl;
      }

      char* send_labels[SEND_LABELS.size()];
      std::copy(SEND_LABELS.begin(), SEND_LABELS.end(), send_labels);

      char* receive_labels[RECV_LABELS.size()];
      std::copy(RECV_LABELS.begin(), RECV_LABELS.end(), receive_labels);

      std::cout << "\nSend Labels: " << std::endl;
      for (auto i = SEND_LABELS.begin(); i != SEND_LABELS.end(); ++i){
        std::cout << *i << " " << std::endl;
      }
      std::cout << "\nReceive Labels: " << std::endl;
      for (auto i = RECV_LABELS.begin(); i != RECV_LABELS.end(); i++){
        std::cout << *i << " " << std::endl;
      }
      std::cout << std::endl;

      this->connection = new SpynnakerLiveSpikesConnection(RECV_LABELS.size(), receive_labels, SEND_LABELS.size(), send_labels, (char*) local_host, local_port);
      std::cout << "Spynnaker Interface ... connection " << std::endl;

      // Create the spinnaker interface
      this->spikes_interface = new SpikesCallbackInterface(spinnName, spikes, wait);
      std::cout << "Spynnaker Interface ... interface " << std::endl;

      for (std::map<std::string, SpikesPopulation*>::iterator it = spikes.begin(); it!=spikes.end(); ++it) {

        this->connection->add_initialize_callback((char*)it->second->getPopLabel().c_str(), spikes_interface);

        if( strcmp(it->second->getPopType().c_str(),"injector")==0 )
          this->connection->add_start_callback((char*)it->second->getPopLabel().c_str(), spikes_interface);
        if( strcmp(it->second->getPopType().c_str(),"receiver")==0 )
          this->connection->add_receive_callback((char*)it->second->getPopLabel().c_str(), spikes_interface);
      }

      std::cout << "Spynnaker Interface ... set callbacks " << std::endl;

      if (absolute_file_path != NULL) {
        this->connection->set_database(absolute_file_path);
      }

      success = true;
      std::cout << "Spynnaker Live Spikes Connection set. \n Waiting for database to be ready ... " << std::endl;

    } catch (char const* msg){
      printf("%s \n", msg);
      success = false;
    }

    return success;
}


/******************************************************************************/
bool SpinnakerInterface::interruptModule()
{
    //outputManager.stop();
    //inputManager.interrupt();
    yarp::os::RFModule::interruptModule();
    return true;
}

/******************************************************************************/
bool SpinnakerInterface::close()
{
    //outputManager.threadRelease();
    //inputManager.close();
    yarp::os::RFModule::close();
    return true;
}

/******************************************************************************/
bool SpinnakerInterface::respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
    reply.clear();

    if (command.get(0).asString() == "help") {
      reply.addString("help: start (This will start live spikes when database is ready)");
      reply.addString("help: quit ");
    } else {
      reply.addString(command.get(0).asString() + "recieved ...");

      if (command.get(0).asString() == "quit") {
        reply.addString("command not recognised");
        close(); //this must change?
      } else if (command.get(0).asString() == "start") {
        spikes_interface->startSpikesInterface();
        reply.addString("Ready to start the simulation ... ");
      } else { reply.addString("command not recognised"); }
    }
    return true;
}

/******************************************************************************/
bool SpinnakerInterface::updateModule()
{
    return true;
}

/******************************************************************************/
double SpinnakerInterface::getPeriod()
{
    return 1.0;
}


/******************************************************************************/
// SPIKES_CALLBACK_INTERFACE - IO
/******************************************************************************/
SpikesCallbackInterface::SpikesCallbackInterface(std::string name, std::map<std::string, SpikesPopulation*> &spikes, bool wait_for_start)
{
  spinInterfaceName = name;
  ready_to_start = !wait_for_start;
  simulation_started = false;
  database_read = false;

  spikes_structure = spikes;

  n_populations_to_read = spikes.size();

  //create i:o: ports from spikes structure

  if (pthread_mutex_init(&(this->start_mutex), NULL) == -1) {
      fprintf(stderr, "Error initializing start mutex!\n");
      exit(-1);
  }
  if (pthread_cond_init(&(this->start_condition), NULL) == -1) {
      fprintf(stderr, "Error initializing start condition!\n");
      exit(-1);
  }

  if (pthread_mutex_init(&(this->point_mutex), NULL) == -1) {
      fprintf(stderr, "Error initializing point mutex!\n");
      exit(-1);
  }


}
/******************************************************************************/
void SpikesCallbackInterface::init_population(char *label, int n_neurons, float run_time_ms, float machine_time_step_ms)
{
  std::string label_str = std::string(label);
  spikes_structure[label_str]->initPopulation(n_neurons, run_time_ms, machine_time_step_ms);
  if (spikes_structure[label_str]->setPopulationPort(spinInterfaceName, true)){
    std::cout << "Population " << label_str << " initialise ... waiting for database " << std::endl;
  }

  pthread_mutex_lock(&(this->start_mutex));
  this->n_populations_to_read -= 1;
  if (this->n_populations_to_read <= 0) {
      this->database_read = true;
      while (!this->ready_to_start) {
          pthread_cond_wait(&(this->start_condition), &(this->start_mutex));
      }
  }
  pthread_mutex_unlock(&(this->start_mutex));

}
/******************************************************************************/
void SpikesCallbackInterface::spikes_start(char *label, SpynnakerLiveSpikesConnection *connection)
{
  std::vector<int> n_neuron_ids; //send_spikes parameter
  bool send_full_keys=false; //send_spikes parameter

  std::string label_str = std::string(label);

  while (true) {
    n_neuron_ids = spikes_structure[label_str]->spikesToSpinnaker();
    if (!n_neuron_ids.empty()) {
      //pthread_mutex_lock(&(this->start_mutex));
      this->simulation_started = true;
      connection->send_spikes(label, n_neuron_ids, send_full_keys);
      //pthread_mutex_unlock(&(this->start_mutex));
    }
    n_neuron_ids.clear();
  }
  //n_neuron_ids = spikes_structure[label_str]->spikesToSpinnaker();


}
/******************************************************************************/
void SpikesCallbackInterface::receive_spikes(char *label, int time, int n_spikes, int* spikes)
{
  //pthread_mutex_lock(&(this->point_mutex));
  std::string label_str = std::string(label);

  spikes_structure[label_str]->spikesToYarpPort( time, n_spikes, spikes);
  //pthread_mutex_unlock(&(this->point_mutex));
}

/******************************************************************************/
void SpikesCallbackInterface::startSpikesInterface() {
  pthread_mutex_lock(&(this->start_mutex));
  if (!this->ready_to_start) {
      std::cout << "Starting the simulation ..." << std::endl;
      this->ready_to_start = true;
      pthread_cond_signal(&(this->start_condition));
  }
  pthread_mutex_unlock(&(this->start_mutex));
}


//empty line to make gcc happy
