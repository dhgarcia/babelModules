


#include "iCub_lip_gazeCtrl.h"


bool lipGazeCtrlModule::configure(yarp::os::ResourceFinder &rf)
{
    //set the name of the module
    moduleName =
            rf.check("name", yarp::os::Value("/lipGazeCtrl")).asString();
    setName(moduleName.c_str());

    //base name of the robot for the camera ports /icub or /icubSim
    //std::string robotName = rf.check("robot", yarp::os::Value("/icub")).asString();

    int retWidth = rf.check("width", yarp::os::Value(32)).asInt();
    int retHeight = rf.check("height", yarp::os::Value(32)).asInt();

    //int imgWidth = rf.check("icub_width", yarp::os::Value(320)).asInt();
    //int imgHeight = rf.check("icub_height", yarp::os::Value(240)).asInt();

    bool strict = rf.check("strict") &&
            rf.check("strict", yarp::os::Value(true)).asBool();
    bool position = rf.check("position") &&
            rf.check("position", yarp::os::Value(true)).asBool();

    // Open
    lipCtrl = new lipGazeCtrl(moduleName, retWidth, retHeight);
    if(!lipCtrl->open(moduleName, strict, position))
        return false;

    return true;
}

/******************************************************************************/
bool lipGazeCtrlModule::interruptModule()
{
    yarp::os::RFModule::interruptModule();
    return true;
}

/******************************************************************************/
bool lipGazeCtrlModule::close()
{
    yarp::os::RFModule::close();
    return true;
}

/******************************************************************************/
bool lipGazeCtrlModule::updateModule()
{
    if(isStopping()) return false;

    return true;
}

/******************************************************************************/
double lipGazeCtrlModule::getPeriod()
{
    return 1.0;
}

/******************************************************************************/

lipGazeCtrlModule::~lipGazeCtrlModule(){

}

/******************************************************************************/
// lipGazeCtrl STUFF
/******************************************************************************/

lipGazeCtrl::lipGazeCtrl(const std::string &name, int width, int height){

  // inti Value
  retina_width = width;
  retina_height = height;
  lip_width = retina_width/1.6/2; //10;
  lip_height = retina_height/1.6/2; //10
  icub_img_width = 320; //128
  icub_img_height = 240; //128;
  //init maps
  InputIDMap = createIDMap(retina_height, retina_width);

  input_x_array = new int [retina_width*retina_height];
  input_y_array = new int [retina_width*retina_height];
  coordMappingArray(input_x_array, input_y_array, retina_height, retina_width);
  printf("test y %d, x %d, nrn %d\n", retina_height-1,retina_width-1, InputIDMap[retina_height-1][retina_width-1]);
  printf("test nrn %d, y %d, x %d\n", InputIDMap[retina_height-1][retina_width-1], input_y_array[InputIDMap[retina_height-1][retina_width-1]],input_x_array[InputIDMap[retina_height-1][retina_width-1]]); //this should be changed to retina_width-1 retina_height-1

  lip_x_array = new int [lip_width*lip_height];
  lip_y_array = new int [lip_width*lip_height];
  coordMappingArray(lip_x_array, lip_y_array, lip_height, lip_width);


  //gazecontrollerclient
  optGaze.put("device","gazecontrollerclient");
  optGaze.put("remote","/iKinGazeCtrl");
  optGaze.put("local", name + "/gaze:o");
  yarp::dev::PolyDriver clientGazeCtrl(optGaze);
  //yarp::dev::IGazeControl *igaze=NULL;
  if (clientGazeCtrl.isValid()) {
    clientGazeCtrl.view(igaze);
  }
}

/******************************************************************************/
bool lipGazeCtrl::open(const std::string &name, bool strictio, bool sendPosition)
{
    //and open the input port
    if(strictio) this->setStrict();
    this->strictio = strictio;
    this->useCallback();

    yarp::os::BufferedPort< yarp::os::Bottle >::open(name + "/lip:i");

    if(sendPosition) outPort.open(name + "/position:o");
    this->sendPosition = sendPosition;

    return true;
}

/******************************************************************************/
void lipGazeCtrl::close()
{
  outPort.close();
  yarp::os::BufferedPort<yarp::os::Bottle >::close();
  //remember to also deallocate any memory allocated by this class
}

/******************************************************************************/
void lipGazeCtrl::interrupt()
{
  outPort.interrupt();
  yarp::os::BufferedPort<yarp::os::Bottle >::interrupt();
}

/******************************************************************************/
void lipGazeCtrl::onRead(yarp::os::Bottle &lip)
{
  std::cout << "Got some LIP saliency data..... " << lip.size() << std::endl;
  //lipInList->size() > 1

  // get the item back from the Bottle
  yarp::os::Value tmpVal = lip.pop();
  // Convert to integer neuron ID
  int nrnId = tmpVal.asInt();
  std::cout << "Salient nrn ID: " << nrnId << "," << lip_x_array[nrnId] << "," << lip_y_array[nrnId] << std::endl;

  if (nrnId == 0) return; //break;

  // Now some simple calculations to work out the salient location
  // in iCub's view from the neuron id
  // calculate the magnification from lip up to input
  // layers in the neural network
  int lip_mag = floor(((float)retina_width/(float)retina_width)); //shouldn't it be retina_height?

  // Use the mag to convert from LIP coords to input coords
  double x_attend = double(lip_x_array[nrnId]*lip_mag);
  double y_attend = double(lip_y_array[nrnId]*lip_mag);
  std::cout << "Attend to position (input space) " << x_attend << "," << y_attend << std::endl;//<< lip_mag << "," << x_attend << "," << y_attend << std::endl;

  // calculate the magnification from input layer in the neural network
  // to iCub 320x240 view
  double icub_view_xmag = (int)icub_img_width/(int)retina_width;
  double icub_view_ymag = (int)icub_img_height/(int)retina_height;
  // Use the mag to convert input coords to iCub view coords
  double loc_x = x_attend*icub_view_xmag;
  double loc_y = y_attend*icub_view_ymag;
  std::cout << "Location to attend is: " << loc_x << "," << loc_y << std::endl;

  yarp::sig::Vector x;
  igaze->getFixationPoint(x);  // retrieve the current fixation point

  int camSel=0;   // select the image plane: 0 (left), 1 (right)
  yarp::sig::Vector px(2);   // specify the pixel where to look
  px[0]=loc_x; //160.0; //loc_x;
  px[1]=loc_y; //120.0; //loc_y;
  double z=1.0;   // distance [m] of the object from the image plane (extended to infinity): yes, you probably need to guess, but it works pretty robustly
  igaze->lookAtMonoPixel(camSel,px,z);    // look!

  // RETURN GAZE TO CENTER OF IMAGE //REMOVE FOR REAL iCUB tracking ctrl
  yarp::os::Time::delay(5.0);
  px[0]=(int)icub_img_width/2; //160.0; //loc_x;
  px[1]=(int)icub_img_height/2; //120.0; //loc_y;
  igaze->lookAtMonoPixel(camSel,px,z);    // look!
  igaze->lookAtFixationPoint(x);             // request to gaze at the desired fixation point and wait for reply (sync method)
  igaze->waitMotionDone();
  std::cout << "Go back to: " << x[0] << "," << x[1] << "," << x[2] << std::endl;

  // terminate here - continuous operation TODO!
  //break;

  //send positions to iCub for arm control
  if (sendPosition) {
    yarp::os::Bottle& posList = outPort.prepare();
    posList.clear();
    posList.addDouble(loc_x);
    posList.addDouble(loc_y);
    posList.addDouble(z);
    if(strictio) outPort.writeStrict();
    else outPort.write();
  }
}

/******************************************************************************/

// Create a 2D vector to map x,y pos to a neuron ID
std::vector<std::vector<int> > lipGazeCtrl::createIDMap(int height, int width){

  std::vector<std::vector<int> > IDMap;
  int current_nrn = 0;
  int i,j;

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

void lipGazeCtrl::coordMappingArray(int input_x_array[], int input_y_array[], int height, int width){
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
