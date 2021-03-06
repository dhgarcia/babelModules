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
SpikesPopulation::SpikesPopulation(std::string label, std::string type, bool strict, bool broadcast){
  this->population_label = label;
  this->population_type = type;

  this->strictio = strict;
  this->broadcast = broadcast;
}
/******************************************************************************/
void SpikesPopulation::setPopSize(int x, int y)
{
  this->population_size[0]=x;
  this->population_size[1]=y;
}
/******************************************************************************/
void SpikesPopulation::initSpinnakerPopulation(int n_neurons, float run_time_ms, float machine_time_step_ms)
{
  this->n_neurons = n_neurons;
  this->run_time_ms = run_time_ms;
  this->machine_time_step_ms = machine_time_step_ms;
}

/******************************************************************************/
std::string SpikesPopulation::getPopLabel()
{
  return this->population_label;
}
/******************************************************************************/
std::string SpikesPopulation::getPopType()
{
  return this->population_type;
}
/******************************************************************************/
bool  SpikesPopulation::connectSpikePortToCallbackPort( yarp::os::BufferedPort<yarp::os::Bottle>* readPort)
{
  callbackSpikesPort = readPort;

  return yarp::os::Network::connect(this->spikesPort.getName(), callbackSpikesPort->getName(), "udp");
}
/******************************************************************************/
void SpikesPopulation::run() {
  //while (true) {
    //std::cout << this->getPopLabel() << std::endl;
  //}
}
/******************************************************************************/
bool SpikesPopulation::threadInit() {
  return true;
}

/******************************************************************************/
// SPIKES_RECEIVER_POPULATION
/******************************************************************************/
SpikesReceiverPopulation::SpikesReceiverPopulation(std::string label, std::string type, bool strict, bool broadcast) : SpikesPopulation(label, type, strict, broadcast) {

}
/******************************************************************************/
void SpikesReceiverPopulation::spikesFromSpinnaker(int time, int n_spikes, int* spikes){

  yarp::os::Bottle& spikeList = this->spikesPort.prepare();
  spikeList.clear();

  for (int neuron_id_position = 0;  neuron_id_position < n_spikes; neuron_id_position++)
  {
    std::cout << "Received spike at time" << time << ", from " << this->population_label << " - " << spikes[neuron_id_position] << std::endl;
    spikeList.addInt(spikes[neuron_id_position]);
  }

  //std::cout << "Length of receive spike list is " << spikeList.size() << std::endl;
  if(strictio) this->spikesPort.writeStrict();
  else this->spikesPort.write();

}
/******************************************************************************/
std::vector<int> SpikesReceiverPopulation::spikesToSpinnaker(){
  return std::vector<int>();
}
/******************************************************************************/
bool SpikesReceiverPopulation::connectToSourcePort(){
  return false;
}

/******************************************************************************/
bool SpikesReceiverPopulation::setPopulationPorts(std::string moduleName, yarp::os::BufferedPort<yarp::os::Bottle>* readPort)
{
  //if (broadcast)
  this->spikesPort.open(moduleName + "/" +  this->population_label + "/" + this->population_type + ":o");
  //this->strictio = broadcast;

  //if(strictio) this->spikesPort.setStrict();
  this->strictio = strictio;

  connectSpikePortToCallbackPort(readPort);

  return true;
}

/******************************************************************************/
// SPIKES_RECEIVER_POPULATION
/******************************************************************************/
SalientSpikesReceiverPopulation::SalientSpikesReceiverPopulation(std::string label, std::string type, int ret_width, int ret_height, int robot_img_width, int robot_img_height, bool strict, bool broadcast) : SpikesReceiverPopulation(label, type, strict, broadcast) {


  retina_width = ret_width;
  retina_height = ret_height;
  this->setPopSize(retina_width/1.6/2, retina_height/1.6/2);

  source_img_width = robot_img_width;
  source_img_height = robot_img_height;

  //InputIDMap = createIDMap(retina_height, retina_width);

  lip_x_array = new int [this->population_size[0]*this->population_size[1]];
  lip_y_array = new int [this->population_size[0]*this->population_size[1]];
  coordMappingArray(lip_x_array, lip_y_array, this->population_size[0], this->population_size[1]);


}
/******************************************************************************/
void SalientSpikesReceiverPopulation::spikesFromSpinnaker(int time, int n_spikes, int* spikes){

  yarp::os::Bottle& posList = this->spikesPort.prepare();
  posList.clear();

  if (n_spikes >= 2) return; //skip if we get more than one salient point?

  int nrnId = spikes[0];

  int lip_mag = floor(((float)retina_width/(float)retina_height)); 
  // Use the mag to convert from LIP coords to input coords
  double x_attend = double(lip_x_array[nrnId]*lip_mag);
  double y_attend = double(lip_y_array[nrnId]*lip_mag);
  std::cout << "Attend to position (input space) " << x_attend << "," << y_attend << std::endl;//<< lip_mag << "," << x_attend << "," << y_attend << std::endl;

  // calculate the magnification from input layer in the neural network
  // to iCub 320x240 view
  double source_view_xmag = (int)source_img_width/(int)retina_width;
  double source_view_ymag = (int)source_img_height/(int)retina_height;
  // Use the mag to convert input coords to iCub view coords
  double loc_x = x_attend*source_view_xmag;
  double loc_y = y_attend*source_view_ymag;
  std::cout << "Location to attend is: " << loc_x << "," << loc_y << std::endl;

  posList.addDouble(loc_x);
  posList.addDouble(loc_y);
  posList.addDouble(1.0);

  if(strictio) this->spikesPort.writeStrict();
  else this->spikesPort.write();

}
/******************************************************************************/
// std::vector<std::vector<int> > SalientSpikesReceiverPopulation::createIDMap(int height, int width){
//   // Create a 2D vector to map x,y pos to a neuron ID
//
//   std::vector<std::vector<int> > IDMap;
//   int current_nrn = 0;
//   int i,j;
//
//   // set 'row' dimension
//   IDMap.resize(height);
//
//   // set 'column' dimension for each row and
//   // set the neuron ID for each element
//
//   // When assigning neuron IDs we have to be careful
//   // to match the coordinate system of the network.
//   // For the PyNN version nrn 0 is top left hand corner
//   // and increments across the row and down the column
//   // This matches exactly the image x,y coordinates
//
//   // i is the row coord and j is the x coord
//   for( int i=0; i< height; i++){
//     IDMap[i].resize(width);
//     for(int j=0; j < width; j++){
//       //printf("%d %d %d\n", i,j, current_nrn);
//       IDMap[i][j] = current_nrn;
//       current_nrn++;
//     }
//   }
//
//   return IDMap;
// }

/******************************************************************************/
void SalientSpikesReceiverPopulation::coordMappingArray(int input_x_array[], int input_y_array[], int width, int height){
  int startx = 0;
  int starty = 0;
  //int input_x_array [width*height];
  //int input_y_array [width*height];

  for(int n = 0; n < (width*height); n++){
    input_x_array[n] = startx;
    input_y_array[n] = starty;
    //printf("%d %d %d\n", startx,starty, n);
    if (startx == (width - 1)){
      startx = 0;
      starty = starty + 1;
    } else {
      startx = startx + 1;
    }
  }

}


/******************************************************************************/
// SPIKES_INJECTOR_POPULATION
/******************************************************************************/
template <typename T>
SpikesInjectorPopulation<T>::SpikesInjectorPopulation(std::string label, std::string type, int width, int height, std::string source, bool strict, bool broadcast) : SpikesPopulation(label, type, strict, broadcast) {
  //this->population_label = label;
  //this->population_type = type;

  this->population_size[0]=width;
  this->population_size[1]=height;

  this->sourcePortName = source;

  //this->open("/spinnakerIO", true, false);
  //yarp::os::Network::connect(sourcePortName.c_str(), yarp::os::BufferedPort< T >::getName());

}
/******************************************************************************/
template <typename T>
bool SpikesInjectorPopulation<T>::setPopulationPorts(std::string moduleName, yarp::os::BufferedPort<yarp::os::Bottle>* readPort) {
  if (this->open(moduleName, broadcast, strictio)) {
    if ( connectToSourcePort() ) {
      return connectSpikePortToCallbackPort(readPort);
    }
  }

  return false;
}
/******************************************************************************/
template <typename T>
bool SpikesInjectorPopulation<T>::open(const std::string &name, bool strictio, bool broadcast)
{
    //and open the input port
    if(strictio) this->setStrict();
    this->strictio = strictio;
    this->useCallback();

    if ( yarp::os::BufferedPort<T>::open(name + "/" +  this->population_label + "/" + this->population_type + ":i") ) {
      //std::cout << "Open port " << name << "/" << this->population_label << "/" << this->population_type << ":i" << std::endl;
    }

    if (broadcast) viewerPort.open(name + "/" +  this->population_label + "/source");
    this->broadcast = broadcast;

    spikesPort.open(name + "/" +  this->population_label + "/" + this->population_type + ":o");

    return true;
}
/******************************************************************************/
template <typename T>
void SpikesInjectorPopulation<T>::close()
{
  spikesPort.close();
  if (broadcast) viewerPort.close();
  yarp::os::BufferedPort<T>::close();
  //remember to also deallocate any memory allocated by this class
  this->stop();//stop thread?
}

/******************************************************************************/
template <typename T>
void SpikesInjectorPopulation<T>::interrupt()
{
  spikesPort.interrupt();
  if (broadcast) viewerPort.interrupt();
  yarp::os::BufferedPort<T>::interrupt();
}
/******************************************************************************/
template <typename T>
bool SpikesInjectorPopulation<T>::connectToSourcePort()
{
  return yarp::os::Network::connect(sourcePortName.c_str(), yarp::os::BufferedPort< T >::getName(), "udp");
}

/******************************************************************************/
template <typename T>
void SpikesInjectorPopulation<T>::spikesFromSpinnaker(int time, int n_spikes, int* spikes){

}
/******************************************************************************/
template <typename T>
std::vector<int> SpikesInjectorPopulation<T>::spikesToSpinnaker(){

  yarp::os::Bottle *bottle = this->callbackSpikesPort->read(this->strictio);
  if (bottle!=NULL) {
    std::vector<int> n_neuron_ids;

    int n_spikes = bottle->size();
    //std::cout << "Length of spikes is " << n_spikes << std::endl;
    // get the item back from the Bottle
    //std::cout << ++times_onRead << ": ";
    for (int neuron_id_position = 0;  neuron_id_position < n_spikes; neuron_id_position++)
    {
      yarp::os::Value tmpVal = bottle->pop();
      // Convert to integer neuron ID
      int nrnId = tmpVal.asInt();
      n_neuron_ids.push_back(nrnId);
      //std::cout << nrnId << " ";
    }
    //std::cout << ++times_onRead << ": " << "Length of spike list is " << n_neuron_ids.size() << std::endl;
    //std::cout << std::endl;
    // clear the Bottle
    //bottle->clear();

    return n_neuron_ids;
  }
  //std::cout << ++times_onRead << "/* message */" << '\n';
  return std::vector<int>();
}
/******************************************************************************/

/******************************************************************************/
// EVENT_SPIKES_INJECTOR_POPULATION
/******************************************************************************/
EventSpikesInjectorPopulation::EventSpikesInjectorPopulation(std::string label, std::string type, int ev_polarity, int ev_width, int ev_height, int pop_width, int pop_height, bool flip, std::string source, bool strict, bool broadcast) : SpikesInjectorPopulation(label, type, pop_width, pop_height, source, strict, broadcast) {

  this->flip = flip;

  this->ev_polarity = ev_polarity;
  this->ev_width = ev_width;
  this->ev_height = ev_height;

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
    //std::cout << ++times_onRead << ": " << (int)(v->x) << " " << (int)(v->y) << " " << neuronID << std::endl;//<< label << " ID:: " << neuronID << std::endl;
  }

  //connection->send_spikes(label, n_neuron_ids, send_full_keys);
  //n_neuron_ids.clear();

  //send on the processed image
  if(strictio) spikesPort.writeStrict();
  else spikesPort.write();
  spikeList.clear();

}


/******************************************************************************/
// VISION_SPIKES_INJECTOR_POPULATION
/******************************************************************************/
VisionSpikesInjectorPopulation::VisionSpikesInjectorPopulation(std::string label, std::string type, int v_width, int v_height, int pop_width, int pop_height, std::string source, bool strict, bool broadcast) : SpikesInjectorPopulation(label, type, pop_width, pop_height, source, strict, broadcast) {

  //this->flip = flip;

  //this->ev_polarity = ev_polarity;
  this->v_width = v_width;
  this->v_height = v_height;

  this->InputIDMap = createIDMap(v_height, v_width);
}
/******************************************************************************/
void VisionSpikesInjectorPopulation::onRead(yarp::sig::ImageOf<yarp::sig::PixelBgr> &bot)
{

  cv::Mat img = cv::cvarrToMat((IplImage *)bot.getIplImage());
  cv::resize(img, img, cv::Size(population_size[0], population_size[1]));
  /// Convert the image to Gray
  cv::cvtColor( img, img, CV_BGR2GRAY );
  cv::threshold( img, img, 0, 255, cv::THRESH_BINARY);
  // Have to wrap OpenCV images back to yarp images first
  yarp::sig::ImageOf<yarp::sig::PixelBgr>& yarpViewImage = viewerPort.prepare();
  //if (broadcast) yarpViewImage = viewerPort.prepare();
  IplImage iplFrame = IplImage( img );
  yarpViewImage.wrapIplImage(&iplFrame);
  // Write image to YARP viewer port and also out to data port
  if (broadcast) viewerPort.write();

  // prepare the ports for sending
  yarp::os::Bottle& spikeList = spikesPort.prepare();
  // Push the Neuron IDs of 'on' pixels to vector.
  for (int x=0; x<population_size[0]; x++) {
    for (int y=0; y<population_size[1]; y++) {
      yarp::sig::PixelBgr& monopixel = yarpViewImage.pixel(x,y);
      if (monopixel.r > 0 && monopixel.b > 0 && monopixel.g > 0 ) {
        // The pixel is ON, so get neuron ID and push onto vector
        int neuronID = InputIDMap[y][x];
        spikeList.addInt(neuronID);
      }
    } // y pix loop
  } // x pix loop


  //connection->send_spikes(label, n_neuron_ids, send_full_keys);
  //n_neuron_ids.clear();

  //send on the processed image
  if(strictio) spikesPort.writeStrict();
  else spikesPort.write();
  spikeList.clear();

}

/******************************************************************************/
std::vector<std::vector<int> > VisionSpikesInjectorPopulation::createIDMap(int height, int width){

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
// COLOUR_VISION_SPIKES_INJECTOR_POPULATION
/******************************************************************************/
ColourVisionSpikesInjectorPopulation::ColourVisionSpikesInjectorPopulation(std::string label, std::string type, int v_width, int v_height, int pop_width, int pop_height, std::string source, bool strict, bool broadcast) : VisionSpikesInjectorPopulation(label, type, v_width, v_height, pop_width, pop_height, source, strict, broadcast) {

}
/******************************************************************************/
void ColourVisionSpikesInjectorPopulation::onRead(yarp::sig::ImageOf<yarp::sig::PixelBgr> &bot)
{

  cv::Mat img = cv::cvarrToMat((IplImage *)bot.getIplImage());
  cv::resize(img, img, cv::Size(population_size[0], population_size[1]));

  // Split image to colour channels
  cv::Mat imgR, imgG, imgB;
  cv::Mat imgBGR[3];
  cv::split(img, imgBGR);

  /// Convert the image to Gray
  cv::cvtColor( img, img, CV_BGR2GRAY );
  cv::threshold( img, img, 0, 255, cv::THRESH_BINARY);



  // Have to wrap OpenCV images back to yarp images first
  yarp::sig::ImageOf<yarp::sig::PixelBgr>& yarpViewImage = viewerPort.prepare();
  //if (broadcast) yarpViewImage = viewerPort.prepare();
  IplImage iplFrame = IplImage( img );
  yarpViewImage.wrapIplImage(&iplFrame);
  // Write image to YARP viewer port and also out to data port
  if (broadcast) viewerPort.write();

  // prepare the ports for sending
  yarp::os::Bottle& spikeList = spikesPort.prepare();
  // Push the Neuron IDs of 'on' pixels to vector.
  for (int x=0; x<population_size[0]; x++) {
    for (int y=0; y<population_size[1]; y++) {
      yarp::sig::PixelBgr& monopixel = yarpViewImage.pixel(x,y);
      if (monopixel.r > 0 && monopixel.b > 0 && monopixel.g > 0 ) {
        // The pixel is ON, so get neuron ID and push onto vector
        int neuronID = InputIDMap[y][x];
        spikeList.addInt(neuronID);
      }
    } // y pix loop
  } // x pix loop


  //connection->send_spikes(label, n_neuron_ids, send_full_keys);
  //n_neuron_ids.clear();

  //send on the processed image
  if(strictio) spikesPort.writeStrict();
  else spikesPort.write();
  spikeList.clear();

}


/******************************************************************************/
// AUDIO_SPIKES_INJECTOR_POPULATION
/******************************************************************************/
AudioSpikesInjectorPopulation::AudioSpikesInjectorPopulation(std::string label, std::string type, int pop_width, int pop_height, std::string source, bool strict, bool broadcast) : SpikesInjectorPopulation(label, type, pop_width, pop_height, source, broadcast) {

}
/******************************************************************************/
void AudioSpikesInjectorPopulation::onRead(yarp::os::Bottle &bot)
{

  // prepare the ports for sending
  yarp::os::Bottle& spikeList = spikesPort.prepare();

  spikeList.copy(bot);

  /*int botSize = bot.size();
  // get the item back from the Bottle
  for (int i=0; i< botSize; i++){
    //std::cout << "Item " << i << std::endl;
    yarp::os::Value tmpVal = bot.pop();
    // Convert to integer neuron ID
    int neuronID = tmpVal.asInt();
    spikeList.addInt(neuronID);
  }*/
  //connection->send_spikes(label, n_neuron_ids, send_full_keys);
  //n_neuron_ids.clear();

  //send on the processed image
  if(strictio) spikesPort.writeStrict();
  else spikesPort.write();
  spikeList.clear();

}
