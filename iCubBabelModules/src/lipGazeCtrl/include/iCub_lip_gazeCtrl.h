
#ifndef __iCub_lip_gazeCtrl__
#define __iCub_lip_gazeCtrl__


#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#include <math.h>

class lipGazeCtrl: public yarp::os::BufferedPort< yarp::os::Bottle >
{

private:

  //iKinGazeCtrl variables
  yarp::os::Property optGaze;
  yarp::dev::IGazeControl *igaze;

  int retina_width;
  int retina_height;
  int lip_width;
  int lip_height;
  // Full image size
  int icub_img_width;  // = 320; //128
  int icub_img_height; // = 240; //128;

  // // variables used to calculate
  // // the most active LIP neuron
  // int lipcounts[lip_width*lip_height];
  // int maxcount = 0;
  // int activelip = 0;

  std::vector<std::vector<int> > InputIDMap;

  //int lip_x_array [lip_width*lip_height];
  //int lip_y_array [lip_width*lip_height];
  int * lip_x_array;
  int * lip_y_array;
  int * input_x_array;
  int * input_y_array;


  yarp::os::Bottle * outPos;
  /*//! the list of output images
  std::vector<yarp::sig::ImageOf<yarp::sig::PixelBgr> *> outImage;*/
  yarp::os::BufferedPort<yarp::os::Bottle> outPort;

  bool strictio;
  bool sendPosition;

  // Create a 2D vector to map x,y pos to a neuron ID
  std::vector<std::vector<int> > createIDMap(int height, int width);
  // Create some arrays for neuron ID to x-y coord mappings
  // this has to match the cordinate system above so we translate
  // the maximally active LIP neuron ID back into the correct
  // coordinate system
  void coordMappingArray(int input_x_array[], int input_y_array[], int height, int width);

public:

  lipGazeCtrl(const std::string &name, int width, int height);

  bool    open(const std::string &name, bool strictio, bool sendPosition);
  void    close();
  void    interrupt();
  //this is the entry point to your main functionality
  void    onRead(yarp::os::Bottle &lip);



};

class lipGazeCtrlModule : public yarp::os::RFModule
{

private:


    lipGazeCtrl * lipCtrl;

public:

    virtual ~lipGazeCtrlModule();

    //the virtual functions that need to be overloaded
    virtual bool configure(yarp::os::ResourceFinder &rf);
    virtual bool interruptModule();
    virtual bool close();
    virtual double getPeriod();
    virtual bool updateModule();

    std::string moduleName;

};


#endif //iCub_lip_gazeCtrl
