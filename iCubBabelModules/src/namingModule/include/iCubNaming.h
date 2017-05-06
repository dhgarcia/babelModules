
#ifndef __iCubNamingModule__
#define __iCubNamingModule__


#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#include <algorithm>

//class icubNamingCtrl: public yarp::os::BufferedPort< yarp::os::Bottle >
class icubNamingCtrl: public yarp::os::Thread
{

private:

  int lip_width;
  int lip_height;
  // Create a 1D vector to hold word map for LABEL
  // initialise it with "neutral"
  std::vector<std::string> WordLocationMap;
  std::string label;

  // Bottle for receiving word
  yarp::os::Bottle *wordIn = NULL;;
  yarp::os::Bottle *acInList = NULL;

  //  Audio Spikes
  yarp::os::BufferedPort<yarp::os::Bottle> spikeAudOutPort;        // make port for sending auditory spike packets
  yarp::os::BufferedPort<yarp::os::Bottle> spikeAudInPort;         // make port to receive back active AC neurons
  //  Speech Ports
  yarp::os::BufferedPort<yarp::os::Bottle> speechInPort;           // receives a word generated from speech-to-text
  yarp::os::BufferedPort<yarp::os::Bottle> speechOutPort;          // sends a word to text-to-speech

  bool strict;
  bool speech;

public:

  icubNamingCtrl();
  bool initialize(const std::string &name, const std::string &label, int width, int height, bool strict, bool speech);

  void run();
  void threadRelease();
  //void threadInit();

  //bool    open(const std::string &name, bool strictio, bool sendPosition);
  //void    close();
  //void    interrupt();
  //this is the entry point to your main functionality
  //void    onRead(yarp::os::Bottle &lip);



};

/*class speechModule: public yarp::os::BufferedPort< yarp::os::Bottle >
{

private:


public:

  speechModule(const std::string &name, int width, int height);

  bool    open(const std::string &name, bool strictio, bool sendPosition);
  void    close();
  void    interrupt();
  //this is the entry point to your main functionality
  void    onRead(yarp::os::Bottle &lip);

};*/

class iCubNamingModule : public yarp::os::RFModule
{

private:


    icubNamingCtrl namingCtrl;

public:

    virtual ~iCubNamingModule();

    //the virtual functions that need to be overloaded
    virtual bool configure(yarp::os::ResourceFinder &rf);
    virtual bool interruptModule();
    virtual bool close();
    virtual double getPeriod();
    virtual bool updateModule();

    std::string moduleName;

};


#endif //iCubNamingModule
