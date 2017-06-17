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


// Create a 2D vector to map x,y pos to a neuron ID
std::vector<std::vector<int> > createIDMap(int height, int width){

  std::vector<std::vector<int> > IDMap;
  int current_nrn = 0;
  // set 'row' dimension
  IDMap.resize(height);

  // set 'column' dimension for each row and
  // set the neuron ID for each element

  // When assigning neuron IDs we have to be careful
  // to match the coordinate system of the network.
  // For the PyNN version nrn 0 is top left hand corner
  // and increments across the row and down the column
  // This matches exactly the image x,y coordinates

  // i is the row coord and j is the x coord
  for( int i=0; i< height; i++){
    IDMap[i].resize(width);
    for(int j=0; j < width; j++){
      //printf("%d %d %d\n", i,j, current_nrn);
      IDMap[i][j] = current_nrn;
      current_nrn++;
    }
  }


  return IDMap;
}

/******************************************************************************/
// SpinnakerInterface
/******************************************************************************/
bool SpinnakerInterface::configure(yarp::os::ResourceFinder &rf)
{
    //set the name of the module
    moduleName = rf.check("name", yarp::os::Value("/spinnakerIO")).asString();
    setName(moduleName.c_str());

    bool wait = rf.check("wait_for_start") &&
            rf.check("wait_for_start", yarp::os::Value(true)).asBool();


    std::map<std::string, SpikesPopulation*> spikes;

    std::cout << "MAP size:" << spikes.size() << std::endl;

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
    std::cout << "nSpikes " << nSpikes << std::endl;

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
        newPopulation = new EventSpikesInjectorPopulation(label_name, population_type, ev_polarity, ev_width, ev_height, pop_width, pop_height, flip);
      } else if (population_type == "vision_injector") {
        //int img_width = populaion_type_list->get(0).asInt();
        //int img_height = populaion_type_list->get(1).asInt();
        //int pop_width = populaion_type_list->get(2).asInt();
        //int pop_height = populaion_type_list->get(3).asInt();
        //newPopulation = new VisionSpikesInjectorPopulation(label_name, population_type, x, y);
      } else if (population_type == "audio_injector") {
        int x = populaion_type_list->get(0).asInt();
        int y = populaion_type_list->get(1).asInt();
        //newPopulation = new AudioSpikesInjectorPopulation(label_name, population_type, x, y);
      } else {
        newPopulation = NULL; //new SpikesPopulation(label_name, population_type); //abstract cannot be instantiated
      }

      //spikes.push_back(newPopulation);
      spikes[label_name] = newPopulation;
    }

    std::cout << "MAP size:" << spikes.size() << std::endl;
    spikes["rec"]->setPopulationPort(moduleName, true);


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

    std::vector<char*> SEND_LABELS;
    std::vector<char*> RECV_LABELS;
    std::cout << "Spynnaker Interface ... set labels " << std::endl;

    for (std::map<std::string, SpikesPopulation*>::iterator it = spikes.begin(); it!=spikes.end(); ++it) {
      std::cout << "Spynnaker Interface ... iterating " << std::endl;
      std::cout << "Spynnaker Interface ... " << it->first << std::endl;
      SpikesPopulation * newPopulation = it->second;
      //std::cout << "Spynnaker Interface ... " << newPopulation->getPopType().c_str() << std::endl;
      std::string type = newPopulation->getPopType();
      strcmp(it->second->getPopType().c_str(),"injector");
      std::cout << "Spynnaker Interface ... comparing " << std::endl;
      if(strcmp(it->second->getPopType().c_str(),"injector")==0)
        SEND_LABELS.push_back((char*)it->second->getPopLabel().c_str());
      if(strcmp(it->second->getPopType().c_str(),"receiver")==0)
        RECV_LABELS.push_back((char*)it->second->getPopLabel().c_str());
    }

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
        std::cout << "Spynnaker Interface ... iterating " << std::endl;
        std::cout << "Spynnaker Interface ... " << it->first << std::endl;
        SpikesPopulation * newPopulation = it->second;
        //std::cout << "Spynnaker Interface ... " << newPopulation->getPopType().c_str() << std::endl;
        std::string type = newPopulation->getPopType();
        strcmp(it->second->getPopType().c_str(),"injector");
        std::cout << "Spynnaker Interface ... comparing " << std::endl;
        if(strcmp(it->second->getPopType().c_str(),"injector")==0)
          SEND_LABELS.push_back((char*)it->second->getPopLabel().c_str());
        if(strcmp(it->second->getPopType().c_str(),"receiver")==0)
          RECV_LABELS.push_back((char*)it->second->getPopLabel().c_str());
      }


      char* send_labels[SEND_LABELS.size()];
      std::copy(SEND_LABELS.begin(), SEND_LABELS.end(), send_labels);
      char* receive_labels[RECV_LABELS.size()];
      std::copy(RECV_LABELS.begin(), RECV_LABELS.end(), receive_labels);

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
      std::cout << "Spynnaker Live Spikes Connection set. \n Waiting for database to be ready... " << std::endl;

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
    return true;
}

/******************************************************************************/
bool SpinnakerInterface::updateModule()
{
  std::cout << "Why? " << std::endl;
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


}
/******************************************************************************/
void SpikesCallbackInterface::init_population(char *label, int n_neurons, float run_time_ms, float machine_time_step_ms)
{
  std::string label_str = std::string(label);
  //add n_neurons, run_time_ms, machine_time_step_ms to a property find by label
  spikes_structure[label_str]->setPopulationPort(spinInterfaceName, true);


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

  //spikes_structure[label_str]->read();

  //read a port
  //from label on spikes_structure read the port
  // from the bottle that read the port pass the neuron ids
  // yarp::os::Bottle *bottle;
  // bottle = spikes_structure[label]->spikesPort.read(); //??
  // bottle >> n_neuron_ids

  //no mutex and endless while loop while(simulation_started)
  // send_spikes

  pthread_mutex_lock(&(this->start_mutex));
  this->simulation_started = true;
  connection->send_spikes(label, n_neuron_ids, send_full_keys);
  pthread_mutex_unlock(&(this->start_mutex));
}
/******************************************************************************/
void SpikesCallbackInterface::receive_spikes(char *label, int time, int n_spikes, int* spikes)
{
  std::string label_str = std::string(label);

  spikes_structure[label_str]->spikesToYarpPort( time, n_spikes, spikes);

}


/******************************************************************************/
// SPINNAKER_IO - SENDER
/******************************************************************************/

SpikeSenderInterface::SpikeSenderInterface(std::string imgPortName, std::string spPortName, int width, int height, int d_width, int d_height, bool isFlip){
  this->imagePortName = imgPortName;
  this->spikePortName = spPortName;

  //here we should initialise the module
  this->source_height = height; //240;//128; //256;
  this->source_width = width; //304;//128; //256;
  this->downsample_width = d_width;
  this->downsample_height = d_height;
  //this->polarity = desired_polarity;

  this->flip = isFlip;

  this->InputIDMap = createIDMap(d_height, d_width);
}

void SpikeSenderInterface::spikes_start(char *label, SpynnakerLiveSpikesConnection *connection){
  std::vector<int> n_neuron_ids; //send_spikes parameter
  bool send_full_keys=false; //send_spikes parameter

  yarp::os::BufferedPort< yarp::sig::ImageOf<yarp::sig::PixelBgr> > spikesPort;
  spikesPort.open(spikePortName + ":i"); //get address parameter
  yarp::os::Network::connect(this->imagePortName.c_str(), this->spikePortName + ":i");

  yarp::os::BufferedPort< yarp::sig::ImageOf<yarp::sig::PixelBgr> > viewerPort;
  viewerPort.open(spikePortName + ":o"); //get address parameter

  yarp::os::BufferedPort<yarp::os::Bottle> spikeOutPort;
  spikeOutPort.open(spikePortName + "/spikes"); //get address parameter

  // Yarp images
  yarp::sig::ImageOf<yarp::sig::PixelBgr> *trackImage;
  //yarp::sig::ImageOf<yarp::sig::PixelBgr> outImage;

  while (true) {
    //read the port
    trackImage = spikesPort.read(); //??

    if (trackImage!=NULL) { // check we actually got something

      cv::Mat outImage = cv::cvarrToMat((IplImage *)trackImage->getIplImage());
      cv::resize(outImage, outImage, cv::Size(downsample_width, downsample_height));
      /// Convert the image to Gray
      cv::cvtColor( outImage, outImage, CV_BGR2GRAY );
      cv::threshold(outImage, outImage, 0, 255, cv::THRESH_BINARY);
      // Have to wrap OpenCV images back to yarp images first
      yarp::sig::ImageOf<yarp::sig::PixelBgr>& yarpViewImage = viewerPort.prepare();
      IplImage iplFrame = IplImage( outImage );
      yarpViewImage.wrapIplImage(&iplFrame);
      // Write image to YARP viewer port and also out to data port
      viewerPort.write();

      // prepare the ports for sending
      yarp::os::Bottle& spikeLumList = spikeOutPort.prepare();
      // Push the Neuron IDs of 'on' pixels to vector.
      for (int x=0; x<downsample_width; x++) {
        for (int y=0; y<downsample_height; y++) {
          yarp::sig::PixelBgr& monopixel = yarpViewImage.pixel(x,y);
          if (monopixel.r > 0 && monopixel.b > 0 && monopixel.g > 0 ) {
            // The pixel is ON, so get neuron ID and push onto vector
            int neuronID = InputIDMap[y][x];
            spikeLumList.addInt(neuronID);
            n_neuron_ids.push_back(neuronID);
          }
        } // y pix loop
      } // x pix loop
      //if (first) std::cout << "Length of lum spike list is " << spikeLumList.size() << std::endl;
      //spikeOutPort.setStrict();
      spikeOutPort.write();
      spikeLumList.clear();

      connection->send_spikes(label, n_neuron_ids, send_full_keys);
      n_neuron_ids.clear();
    }

  } //while loop


}

/******************************************************************************/



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
// VISION STUFF
/******************************************************************************/

visionSpikeSender::visionSpikeSender(int width, int height){
  InputIDMap = createIDMap(height, width);
  downsample_width = width;
  downsample_height = height;
}

/******************************************************************************/
bool visionSpikeSender::open(const std::string &name, bool strictio, bool broadcast)
{
    //and open the input port
    if(strictio) this->setStrict();
    this->strictio = strictio;
    this->useCallback();

    yarp::os::BufferedPort< yarp::sig::ImageOf<yarp::sig::PixelBgr> >::open(name + "/img:i");

    if (broadcast) viewerPort.open(name + "/img:o");
    this->broadcast = broadcast;

    outPort.open(name + "/img/spikes");

    return true;
}

/******************************************************************************/
void visionSpikeSender::close()
{
  outPort.close();
  if (broadcast) viewerPort.close();
  yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelBgr> >::close();
  //remember to also deallocate any memory allocated by this class
}

/******************************************************************************/
void visionSpikeSender::interrupt()
{
  outPort.interrupt();
  if (broadcast) viewerPort.interrupt();
  yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelBgr> >::interrupt();
}

/******************************************************************************/
void visionSpikeSender::onRead(yarp::sig::ImageOf<yarp::sig::PixelBgr> &frame)
{
  cv::Mat img = cv::cvarrToMat((IplImage *)frame.getIplImage());
  cv::resize(img, img, cv::Size(downsample_width, downsample_height));
  /// Convert the image to Gray
  cv::cvtColor( img, img, CV_BGR2GRAY );
  cv::threshold( img, img, 0, 255, cv::THRESH_BINARY);
  // Have to wrap OpenCV images back to yarp images first
  yarp::sig::ImageOf<yarp::sig::PixelBgr>& yarpViewImage = viewerPort.prepare();
  IplImage iplFrame = IplImage( img );
  yarpViewImage.wrapIplImage(&iplFrame);
  // Write image to YARP viewer port and also out to data port
  viewerPort.write();

  // prepare the ports for sending
  yarp::os::Bottle& spikeLumList = outPort.prepare();
  // Push the Neuron IDs of 'on' pixels to vector.
  for (int x=0; x<downsample_width; x++) {
    for (int y=0; y<downsample_height; y++) {
      yarp::sig::PixelBgr& monopixel = yarpViewImage.pixel(x,y);
      if (monopixel.r > 0 && monopixel.b > 0 && monopixel.g > 0 ) {
        // The pixel is ON, so get neuron ID and push onto vector
        int neuronID = InputIDMap[y][x];
        spikeLumList.addInt(neuronID);
      }
    } // y pix loop
  } // x pix loop
  //if (first) std::cout << "Length of lum spike list is " << spikeLumList.size() << std::endl;
  //send on the processed image
  if(strictio) outPort.writeStrict();
  else outPort.write();
  spikeLumList.clear();

}

//empty line to make gcc happy
