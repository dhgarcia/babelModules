


#include "iCubNaming.h"


bool iCubNamingModule::configure(yarp::os::ResourceFinder &rf)
{
    //set the name of the module
    moduleName =
            rf.check("name", yarp::os::Value("/icubNaming")).asString();
    setName(moduleName.c_str());

    //base name of the robot for the camera ports /icub or /icubSim
    //std::string robotName = rf.check("robot", yarp::os::Value("/icub")).asString();
    std::string label = rf.check("label", yarp::os::Value("object")).asString();

    int lipWidth = rf.check("width", yarp::os::Value(10)).asInt();
    int lipHeight = rf.check("height", yarp::os::Value(10)).asInt();

    //int imgWidth = rf.check("icub_width", yarp::os::Value(320)).asInt();
    //int imgHeight = rf.check("icub_height", yarp::os::Value(240)).asInt();

    bool strict = rf.check("strict") &&
            rf.check("strict", yarp::os::Value(true)).asBool();
    bool speech = rf.check("speech") &&
            rf.check("speech", yarp::os::Value(true)).asBool();

    // Init
    if(!namingCtrl.initialize(moduleName, label, lipWidth, lipHeight, strict, speech))
        return false;
    namingCtrl.run();

    return true;
}

/******************************************************************************/
bool iCubNamingModule::interruptModule()
{
    yarp::os::RFModule::interruptModule();
    return true;
}

/******************************************************************************/
bool iCubNamingModule::close()
{
    namingCtrl.stop();
    yarp::os::RFModule::close();
    return true;
}

/******************************************************************************/
bool iCubNamingModule::updateModule()
{
    if(isStopping()) return false;

    return true;
}

/******************************************************************************/
double iCubNamingModule::getPeriod()
{
    return 1.0;
}

/******************************************************************************/

iCubNamingModule::~iCubNamingModule(){

}

/******************************************************************************/
// lipGazeCtrl STUFF
/******************************************************************************/

icubNamingCtrl::icubNamingCtrl(){

}

/******************************************************************************/

bool icubNamingCtrl::initialize(const std::string &name, const std::string &label, int width, int height, bool strict, bool speech){

  // inti Value
  lip_width = width;
  lip_height = height;

  spikeAudOutPort.open(name + "/auditory:o");
  spikeAudInPort.open(name + "/auditory:i");

  if (speech) {
    speechInPort.open(name + "/speech:i");
    speechOutPort.open(name + "/speech:o");
  }
  this->speech = speech;
  this->strict = strict;

  //std::string word = "neutral";
  this->label = label;
  WordLocationMap.resize(lip_width*lip_height);
  for(int i=0; i < lip_width*lip_height; i++){
    WordLocationMap[i] = "None";
  }
  for (int s=10; s <14; s++){
    WordLocationMap[s] = "Vertical";
  }
  for (int s=35; s <39; s++){
    WordLocationMap[s] = "Horizontal";
  }

  return true;
}

/******************************************************************************/
void icubNamingCtrl::run(){

  while(!isStopping()) {

    //
    //yarp::os::Bottle *wordIn = NULL; //this is something the speech module will provide
    //yarp::os::Bottle *acInList = NULL;

    // prepare the ports for sending
    yarp::os::Bottle& spikeAudList = spikeAudOutPort.prepare();
    spikeAudList.clear();
    //word = "Vertical";

    if (speech) {
      // Check for speech in
      wordIn = speechInPort.read(false);
      //wordIn = speech.getWordBottle();
      if (wordIn != NULL){
        label = wordIn->get(0).asString().c_str();
        //std::cout << "Word received " << word << std::endl;
      }
    }
    std::transform(label.begin(), label.end(), label.begin(), ::toupper);
    //std::cout << "Word received " << label << std::endl;

    if (label == "VERTICAL"){
      // Add the name for the stimulus to the word map
      for (int s=10; s <14; s++){
        spikeAudList.addInt(s);
        WordLocationMap[s] = label;
      }
    } else if (label == "HORIZONTAL"){
      // Add the name for the stimulus to the word map
      for (int s=35; s <39; s++){
        spikeAudList.addInt(s);
        WordLocationMap[s] = label;
      }
    } else {
      // Add the name for the stimulus to the word map
      for (int s=80; s <84; s++){
        spikeAudList.addInt(s);
        WordLocationMap[s] = label;
      }
    }

    //std::cout << "Length of aud spike list is " << spikeAudList.size() << std::endl;
    if(strict) spikeAudOutPort.setStrict();
    else spikeAudOutPort.write();


    acInList = spikeAudInPort.read(false);
    if (acInList!=NULL) {
      std::cout << "Got some auditory data..... " << acInList->size() << std::endl;
      int acInSize = acInList->size();
      // get the item back from the Bottle
      for (int i=0; i< acInSize; i++){
        //std::cout << "Item " << i << std::endl;
        yarp::os::Value tmpVal = acInList->pop();
        // Convert to integer neuron ID
        int acNrnId = tmpVal.asInt();
        //std::cout << "Auditory nrn " << acNrnId << ", " << WordLocationMap[acNrnId].c_str() << std::endl;
        //std::cout << "Auditory: " << acNrnId << ", " << WordLocationMap[acNrnId].c_str() << std::endl;

        if (speech) {
          // send the word to text-to-speech
          yarp::os::Bottle &tempOut = speechOutPort.prepare();
          tempOut.clear();
          //tempOut.addString("This ");
          //tempOut.addString("is ");
          tempOut.addString(WordLocationMap[acNrnId].c_str());
          tempOut.addString(" Object");
          speechOutPort.write();
        }

      }
      // clear the Bottle
      acInList->clear();

    }


  }

}


/******************************************************************************/
void icubNamingCtrl::threadRelease()
{
    spikeAudOutPort.close();
    spikeAudInPort.close();
    if(speech){
      speechInPort.close();
      speechOutPort.close();
    }
    //stop();
}

/******************************************************************************/
// bool icubNamingCtrl::open(const std::string &name, bool strictio, bool sendPosition)
// {
//     //and open the input port
//     if(strictio) this->setStrict();
//     this->strictio = strictio;
//     this->useCallback();
//
//     yarp::os::BufferedPort< yarp::os::Bottle >::open(name + "/lip:i");
//
//     if(sendPosition) outPort.open(name + "/position:o");
//     this->sendPosition = sendPosition;
//
//     return true;
// }
//
// /******************************************************************************/
// void icubNamingCtrl::close()
// {
//   outPort.close();
//   yarp::os::BufferedPort<yarp::os::Bottle >::close();
//   //remember to also deallocate any memory allocated by this class
// }
//
// /******************************************************************************/
// void icubNamingCtrl::interrupt()
// {
//   outPort.interrupt();
//   yarp::os::BufferedPort<yarp::os::Bottle >::interrupt();
// }
//
// /******************************************************************************/
// void icubNamingCtrl::onRead(yarp::os::Bottle &lip)
// {
//   std::cout << "Got some LIP saliency data..... " << lip.size() << std::endl;
//   //lipInList->size() > 1
//
//   // get the item back from the Bottle
//   yarp::os::Value tmpVal = lip.pop();
//   // Convert to integer neuron ID
//   int nrnId = tmpVal.asInt();
//   std::cout << "Salient nrn ID: " << nrnId << "," << lip_x_array[nrnId] << "," << lip_y_array[nrnId] << std::endl;
//
//   if (nrnId == 0) return; //break;
//
//   // Now some simple calculations to work out the salient location
//   // in iCub's view from the neuron id
//   // calculate the magnification from lip up to input
//   // layers in the neural network
//   int lip_mag = floor(((float)retina_width/(float)retina_width)); //should it be retina_height?
//
//   // Use the mag to convert from LIP coords to input coords
//   double x_attend = double(lip_x_array[nrnId]*lip_mag);
//   double y_attend = double(lip_y_array[nrnId]*lip_mag);
//   std::cout << "Attend to position (input space) " << x_attend << "," << y_attend << std::endl;//<< lip_mag << "," << x_attend << "," << y_attend << std::endl;
//
//   // calculate the magnification from input layer in the neural network
//   // to iCub 320x240 view
//   double icub_view_xmag = (int)icub_img_width/(int)retina_width;
//   double icub_view_ymag = (int)icub_img_height/(int)retina_height;
//   // Use the mag to convert input coords to iCub view coords
//   double loc_x = x_attend*icub_view_xmag;
//   double loc_y = y_attend*icub_view_ymag;
//   std::cout << "Location to attend is: " << loc_x << "," << loc_y << std::endl;
//
//   yarp::sig::Vector x;
//   igaze->getFixationPoint(x);  // retrieve the current fixation point
//
//   int camSel=0;   // select the image plane: 0 (left), 1 (right)
//   yarp::sig::Vector px(2);   // specify the pixel where to look
//   px[0]=loc_x; //160.0; //loc_x;
//   px[1]=loc_y; //120.0; //loc_y;
//   double z=1.0;   // distance [m] of the object from the image plane (extended to infinity): yes, you probably need to guess, but it works pretty robustly
//   igaze->lookAtMonoPixel(camSel,px,z);    // look!
//
//   yarp::os::Time::delay(5.0);
//   px[0]=(int)icub_img_width/2; //160.0; //loc_x;
//   px[1]=(int)icub_img_height/2; //120.0; //loc_y;
//   igaze->lookAtMonoPixel(camSel,px,z);    // look!
//   igaze->lookAtFixationPoint(x);             // request to gaze at the desired fixation point and wait for reply (sync method)
//   igaze->waitMotionDone();
//   std::cout << "Go back to: " << x[0] << "," << x[1] << "," << x[2] << std::endl;
//
//   // terminate here - continuous operation TODO!
//   //break;
//
//   //send positions to iCub for arm control
//   if (sendPosition) {
//     yarp::os::Bottle& posList = outPort.prepare();
//     posList.clear();
//     posList.addDouble(loc_x);
//     posList.addDouble(loc_y);
//     posList.addDouble(z);
//     if(strictio) outPort.writeStrict();
//     else outPort.write();
//   }
// }
//
/******************************************************************************/
