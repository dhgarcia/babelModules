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
      std::cout << getName() << ": unale to open port" << '\n';
      return false;
    }
    attach(handlePort);

    bool wait = rf.check("wait_for_start") &&
            rf.check("wait_for_start", yarp::os::Value(true)).asBool();


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
        if(populaion_type_list->size() % 2) {
            std::cerr << "Error: receiver spikes incorrectly configured " << std::endl;
            return false;
        }
        int x = populaion_type_list->get(0).asInt();
        int y = populaion_type_list->get(1).asInt();
        newPopulation = new SpikesReceiverPopulation(label_name, population_type);
        newPopulation->setPopSize(x,y);
      } else if (population_type == "event_injector") {
        if(populaion_type_list->size() % 8) {
            std::cerr << "Error: event spikes incorrectly configured " << std::endl;
            return false;
        }
        int ev_polarity = populaion_type_list->get(0).asInt();
        int ev_width = populaion_type_list->get(1).asInt();
        int ev_height = populaion_type_list->get(2).asInt();
        int pop_width = populaion_type_list->get(3).asInt();
        int pop_height = populaion_type_list->get(4).asInt();
        bool flip = populaion_type_list->get(5).asBool();
        std::string source = populaion_type_list->get(6).asString();
        bool strict = populaion_type_list->get(7).asBool();
        newPopulation = new EventSpikesInjectorPopulation(label_name, "injector", ev_polarity, ev_width, ev_height, pop_width, pop_height, flip, source, strict);
      } else if (population_type == "vision_injector") {
        if(populaion_type_list->size() % 6) {
            std::cerr << "Error: vision spikesincorrectly configured " << std::endl;
            return false;
        }
        int img_width = populaion_type_list->get(0).asInt();
        int img_height = populaion_type_list->get(1).asInt();
        int pop_width = populaion_type_list->get(2).asInt();
        int pop_height = populaion_type_list->get(3).asInt();
        std::string source = populaion_type_list->get(4).asString();
        bool strict = populaion_type_list->get(5).asBool();
        newPopulation = new VisionSpikesInjectorPopulation(label_name, "injector", img_width, img_height, pop_width, pop_height, source, strict);
      } else if (population_type == "audio_injector") {
        if(populaion_type_list->size() % 4) {
            std::cerr << "Error: audio spikes incorrectly configured " << std::endl;
            return false;
        }
        int x = populaion_type_list->get(0).asInt();
        int y = populaion_type_list->get(1).asInt();
        std::string source = populaion_type_list->get(2).asString();
        bool strict = populaion_type_list->get(3).asBool();
        newPopulation = new AudioSpikesInjectorPopulation(label_name, "injector", x, y, source, strict);
      } else {
        newPopulation = NULL; //new SpikesPopulation(label_name, population_type); //abstract cannot be instantiated
      }

      spikes[label_name] = newPopulation;
    }

    char const* local_host = NULL;
    char* absolute_file_path = NULL;
    int local_port = rf.check("hand_shake_port",
                         yarp::os::Value(19999),
                         "network input width").asInt();

    std::cout << "Configuring Spynnaker Live Spikes Connection:  "
              << moduleName
              << std::endl;

    // optional variable for not using the live spinnaker IO
    bool spinn = rf.check("no_spinnaker") &&
            rf.check("no_spinnaker", yarp::os::Value(true)).asBool();

    if (!spinn) {
      spinn = initialise(moduleName, wait, (char*) local_host, local_port, absolute_file_path);
    }

    return spinn;
}

/******************************************************************************/
bool SpinnakerInterface::initialise(std::string spinnName, bool wait, char *local_host, int local_port, char* absolute_file_path)
{
  bool success = false;
  std::cout << "Spynnaker Interface ... Initialising " << std::endl;

  try {

    std::vector<char*> SEND_LABELS;
    std::vector<char*> RECV_LABELS;
    std::cout << "Spynnaker Interface ... set labels " << std::endl;

    for (auto const & mp : spikes) {
      std::string label_name(mp.first);
      char *pop_label = new char[label_name.length() + 1];
      std::strcpy(pop_label, label_name.c_str());

      if(strcmp(mp.second->getPopType().c_str(),"injector")==0){
        SEND_LABELS.push_back(pop_label);
      }
      if(strcmp(mp.second->getPopType().c_str(),"receiver")==0){
        RECV_LABELS.push_back(pop_label);
      }
    }

    char* send_labels[SEND_LABELS.size()];
    std::copy(SEND_LABELS.begin(), SEND_LABELS.end(), send_labels);
    std::cout << " Send Labels: " << std::endl;
    for (auto i = SEND_LABELS.begin(); i != SEND_LABELS.end(); ++i){
      std::cout << "   " << *i << " " << std::endl;
    }
    char* receive_labels[RECV_LABELS.size()];
    std::copy(RECV_LABELS.begin(), RECV_LABELS.end(), receive_labels);
    std::cout << " Receive Labels: " << std::endl;
    for (auto i = RECV_LABELS.begin(); i != RECV_LABELS.end(); i++){
      std::cout << "   " << *i << " " << std::endl;
    }

    this->connection = new SpynnakerLiveSpikesConnection(RECV_LABELS.size(), receive_labels, SEND_LABELS.size(), send_labels, (char*) local_host, local_port, false);
    std::cout << "Spynnaker Interface ... connection " << std::endl;

    // Create the spinnaker interface
    this->spikes_interface = new SpikesCallbackInterface(spinnName, spikes, wait);

    std::cout << "Spynnaker Interface ... set callbacks " << std::endl;
    for (auto const & mp : spikes) {
      std::string label_name(mp.first);
      char *pop_label = new char[label_name.length() + 1];
      std::strcpy(pop_label, label_name.c_str());
      this->connection->add_initialize_callback(pop_label, spikes_interface);

      if(strcmp(mp.second->getPopType().c_str(),"injector")==0){
        this->connection->add_start_callback(pop_label, spikes_interface);
        std::cout << "add start callback for " << label_name << std::endl;
      }
      if(strcmp(mp.second->getPopType().c_str(),"receiver")==0){
        this->connection->add_receive_callback(pop_label, spikes_interface);
        std::cout << "add receive callback for " << label_name << std::endl;
      }
    }

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
    reply.addString("help: connect (This will connect spikes port to source)");
    reply.addString("help: quit ");
  } else {
    reply.addString("Got " + command.get(0).asString() + " command ...");

    if (command.get(0).asString() == "quit") {
      reply.addString("Closing ... ");
      close(); //this must change?
    } else if (command.get(0).asString() == "start") {
      spikes_interface->startSpikesInterface();
      reply.addString("Ready to start the simulation ... ");
    } else if (command.get(0).asString() == "connect") {
      spikes_interface->connectSourcePorts();
      reply.addString("Connecting ports ... ");
    } else {
      reply.addString("command not recognised");
    }
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


  if (pthread_mutex_init(&(this->start_mutex), NULL) == -1) {
      fprintf(stderr, "Error initializing start mutex!\n");
      exit(-1);
  }
  if (pthread_cond_init(&(this->start_condition), NULL) == -1) {
      fprintf(stderr, "Error initializing start condition!\n");
      exit(-1);
  }
  if (pthread_mutex_init(&(this->send_mutex), NULL) == -1) {
      fprintf(stderr, "Error initializing send mutex!\n");
      exit(-1);
  }
  if (pthread_mutex_init(&(this->recv_mutex), NULL) == -1) {
      fprintf(stderr, "Error initializing recv mutex!\n");
      exit(-1);
  }


}
/******************************************************************************/
void SpikesCallbackInterface::init_population(char *label, int n_neurons, float run_time_ms, float machine_time_step_ms)
{
  std::string label_str = std::string(label);
  spikes_structure[label_str]->initSpinnakerPopulation(n_neurons, run_time_ms, machine_time_step_ms);

  //create i:o: ports from spikes structure
  yarp::os::BufferedPort<yarp::os::Bottle > *port = new yarp::os::BufferedPort<yarp::os::Bottle >();
  port->open(spinInterfaceName + "/" + label_str + "/reader");
  readPorts.push_back(port);

  pthread_mutex_lock(&(this->start_mutex));

  if (spikes_structure[label_str]->setPopulationPorts(spinInterfaceName, port)){
    std::cout << "Population " << label_str << " initialise ... " << std::endl;
    spikes_structure[label_str]->start();
  } else {
    fprintf(stderr, "Error connecting ports!\n");
    exit(-1);
  }

  this->n_populations_to_read -= 1;
  if (this->n_populations_to_read <= 0) {
    std::cout << " Populations initialised ... Ready to Start ";
    this->database_read = true;
    while (!this->ready_to_start) {
      std::cout << ".";
      pthread_cond_wait(&(this->start_condition), &(this->start_mutex));
    }
    std::cout << "\n Simulation started ... " << std::endl;
    this->simulation_started = true;
  }// else {
    //std::cout << " Not 0!! "<< std::endl;
  //}
  pthread_mutex_unlock(&(this->start_mutex));
}
/******************************************************************************/
void SpikesCallbackInterface::spikes_start(char *label, SpynnakerLiveSpikesConnection *connection)
{
  std::vector<int> n_neuron_ids; //send_spikes parameter
  bool send_full_keys=false; //send_spikes parameter

  std::string label_str = std::string(label);

  std::cout << label_str << " Spikes Start ..." << '\n';
  //this->simulation_started = true;
  while (this->simulation_started) {
    pthread_mutex_lock(&(this->send_mutex));
    n_neuron_ids = spikes_structure[label_str]->spikesToSpinnaker(/*yarp::os::BufferedPort*/);
    if (!n_neuron_ids.empty()) {
      connection->send_spikes(label, n_neuron_ids, send_full_keys);
    }
    n_neuron_ids.clear();
    pthread_mutex_unlock(&(this->send_mutex));
  }
  //n_neuron_ids = spikes_structure[label_str]->spikesToSpinnaker();


}
/******************************************************************************/
void SpikesCallbackInterface::receive_spikes(char *label, int time, int n_spikes, int* spikes)
{
  pthread_mutex_lock(&(this->recv_mutex));
  std::string label_str = std::string(label);
  std::cout << label_str << " Receive Spikes ..." << '\n';

  spikes_structure[label_str]->spikesFromSpinnaker( time, n_spikes, spikes);
  pthread_mutex_unlock(&(this->recv_mutex));
}

/******************************************************************************/
void SpikesCallbackInterface::startSpikesInterface() {
  pthread_mutex_lock(&(this->start_mutex));
  //std::cout << "Waiting for database to be ready ..." << std::endl;
  //while (!this->database_read) {
    //wait!
  //}
  if (!this->ready_to_start) {
    std::cout << "\nStarting the simulation ..." << std::endl;
    this->ready_to_start = true;
    pthread_cond_signal(&(this->start_condition));
  } else {
    std::cout << "Simulation already started." << std::endl;
  }
  pthread_mutex_unlock(&(this->start_mutex));
}

/******************************************************************************/
void SpikesCallbackInterface::connectSourcePorts() {

  for (auto const & mp : spikes_structure) {
    //std::string label_name(mp.first);
    //char *pop_label = new char[label_name.length() + 1];
    //std::strcpy(pop_label, label_name.c_str());

    if(strcmp(mp.second->getPopType().c_str(),"injector")==0){
      mp.second->connectToSourcePort();
    }
    //if(strcmp(mp.second->getPopType().c_str(),"receiver")==0){
      //RECV_LABELS.push_back(pop_label);
    //}
  }

}

//empty line to make gcc happy
