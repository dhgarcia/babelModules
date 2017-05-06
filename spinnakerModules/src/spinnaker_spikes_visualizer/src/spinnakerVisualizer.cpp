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

#include "spinnakerVisualizer.h"
#include <map>
#include <list>
#include <math.h>



/******************************************************************************/
// visualizerSpinnInterface
/******************************************************************************/
bool visualizerSpinnInterface::configure(yarp::os::ResourceFinder &rf)
{
    //set the name of the module
    this->moduleName =
            rf.check("name", yarp::os::Value("visualizer")).asString();
    setName(moduleName.c_str());



    char const* local_host = NULL;
    char* absolute_file_path = NULL;
    char* colour_file_path = NULL;

    int local_port = rf.check("hand_shake_port",
                         yarp::os::Value(19999),
                         "network input width").asInt();

    colour_file_path = rf.check("colour_file").asString().c_str();
    //change colour file to follow vframer example

    std::cout << "Configure Spynnaker Live Spikes Visualization:  "
            //  << moduleName << ", grabber port: " << grabberPortName
              //<< ", input size: " << width << "," << height
              //<< ", downsample size: " << downsample_width << "," << downsample_height
              << "colour file: " << colour_file_path
              << ", local host: " << local_host << ", local port: " << local_port
              << std::endl;

    bool ioSuccess = initSpin(flip, (char*) local_host, local_port, absolute_file_path, colour_file_path);


    return ioSuccess;

}

/******************************************************************************/
bool visualizerSpinnInterface::initSpin(std::string grabberPortName, int width, int height, int d_width, int d_height, bool flip, char *local_host, int local_port, char* absolute_file_path, char* colour_file_path)
{

    bool success = false;

    try {

      // std::vector<char*> SEND_LABELS;
      // std::vector<char*> RECV_LABELS;
      //
      // //labels for spike injection, must be the same name on the other side, labels go in the SPYNNAKER_DATABASE
      // char const* send_label = "spike_injector_ON_0";
      // //char const* receive_label = "LIP_0";
      //
      // SEND_LABELS.push_back((char*)send_label);
      // //RECV_LABELS.push_back((char*)receive_label);
      //
      // char* send_labels[SEND_LABELS.size()];
      // std::copy(SEND_LABELS.begin(), SEND_LABELS.end(), send_labels);
      // char* receive_labels[RECV_LABELS.size()];
      // std::copy(RECV_LABELS.begin(), RECV_LABELS.end(), receive_labels);
      //
      // this->connection = new SpynnakerLiveSpikesConnection(RECV_LABELS.size(), receive_labels, SEND_LABELS.size(), send_labels, (char*) local_host, local_port);
      //
      // std::string spikesPortName = "/" + this->moduleName + "/img";
      // this->sender_callback = new SpikeSenderInterface(grabberPortName, spikesPortName, width, height, d_width, d_height, flip);
      // this->connection->add_start_callback((char *) send_label, sender_callback);
      //
      // //add receiver later on


      //viewer options
      //set up the default channel list
      yarp::os::Bottle tempDisplayList, *bp;
      tempDisplayList.addInt(0);
      tempDisplayList.addString("/Left");
      bp = &(tempDisplayList.addList()); bp->addString("AE");
      tempDisplayList.addInt(1);
      tempDisplayList.addString("/Right");
      bp = &(tempDisplayList.addList()); bp->addString("AE");

      //set the output channels
      yarp::os::Bottle * displayList = rf.find("displays").asList();
      if(!displayList)
          displayList = &tempDisplayList;

      std::cout << displayList->toString() << std::endl;

      if(displayList->size() % 3) {
          std::cerr << "Error: display incorrectly configured in provided "
                       "settings file." << std::endl;
          return false;
      }

      int nDisplays = displayList->size() / 3;

      //for each channel open an vFrame and an output port
      channels.resize(nDisplays);
      outports.resize(nDisplays);
      drawers.resize(nDisplays);

      
      this->colour_reader = new ColourReader(colour_file_path);
      std::vector<char *> *labels = colour_reader->get_labels();

      this->connection = new SpynnakerLiveSpikesConnection((int) labels->size(), &((*labels)[0]), 0, (char **) NULL, (char*) local_host, local_port);

      char *v_argv [1];
      int v_argc=1;
      v_argv [0]=strdup ("visualizeSpikes");
      float ms_per_pixel = 0.0;

      // Create the visualiser
      this->plotter = new RasterPlot(v_argc, v_argv, this->colour_reader, ms_per_pixel, absolute_file_path == NULL);
      for (int i = 0; i < labels->size(); i++) {
        connection.add_initialize_callback((*labels)[i], &plotter);
        connection.add_receive_callback((*labels)[i], &plotter);
        connection.add_start_callback((*labels)[i], &plotter);
      }
      if (absolute_file_path != NULL) {
        connection.set_database(absolute_file_path);
      }
      plotter.main_loop();

      success = true;
      std::cout << "Spynnaker Live Spikes Visualizer set. \n Waiting for database to be ready... " << std::endl;

    } catch (char const* msg){
      printf("%s \n", msg);
      success = false;
    }

    return success;
}


/******************************************************************************/
bool visualizerSpinnInterface::interruptModule()
{
    //outputManager.stop();
    //inputManager.interrupt();
    yarp::os::RFModule::interruptModule();
    return true;
}

/******************************************************************************/
bool visualizerSpinnInterface::close()
{
    //outputManager.threadRelease();
    //inputManager.close();
    yarp::os::RFModule::close();
    return true;
}

/******************************************************************************/
bool visualizerSpinnInterface::updateModule()
{
    return true;
}

/******************************************************************************/
double visualizerSpinnInterface::getPeriod()
{
    return 1;
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
