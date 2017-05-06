

cmake_minimum_required(VERSION 2.6)

set(PROJECTNAME spinnakerModules)
project(${PROJECTNAME})

#YARP
find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include(YarpInstallationHelpers)

#ICUBCONTRIB
find_package(ICUBcontrib)
IF(ICUBcontrib_FOUND)
  list(APPEND CMAKE_MODULE_PATH ${ICUBCONTRIB_MODULE_PATH})
  include(ICUBcontribOptions)
  include(ICUBcontribHelpers)
  icubcontrib_set_default_prefix()
ELSE(ICUBcontrib_FOUND)
  message("Warning: ICUBcontrib not FOUND. Make Install may not install to the correct location")
ENDIF(ICUBcontrib_FOUND)

#SpiNNakerIO
find_package(SpinnakerIO REQUIRED)
#OpenCV
find_package(OpenCV REQUIRED)

#add_subdirectory(app)
add_subdirectory(src)

icubcontrib_add_uninstall_target()
