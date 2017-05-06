# Copyright: (C) 2016 University of Plymouth
# Author: Daniel Hernandez
# CopyPolicy:

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(MODULENAME lipGazeCtrl)
# Start a project
PROJECT(${MODULENAME})

# Find packages
FIND_PACKAGE(YARP REQUIRED)
#FIND_PACKAGE(OpenCV REQUIRED)
LIST(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
INCLUDE(YarpInstallationHelpers)

FIND_PACKAGE(ICUB)
#ICUBCONTRIB
FIND_PACKAGE(ICUBcontrib)
IF(ICUBcontrib_FOUND)
  LIST(APPEND CMAKE_MODULE_PATH ${ICUBCONTRIB_MODULE_PATH})
  INCLUDE(ICUBcontribOptions)
  INCLUDE(ICUBcontribHelpers)
  icubcontrib_set_default_prefix()
ELSE(ICUBcontrib_FOUND)
  message("Warning: ICUBcontrib not FOUND. Make Install may not install to the correct location")
ENDIF(ICUBcontrib_FOUND)

icubcontrib_add_uninstall_target()


set(MY_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR})
# Search for source code.
FILE(GLOB FOLDER_SOURCE ${MY_SRC_PATH}/src/*.cpp)
FILE(GLOB FOLDER_HEADER ${MY_SRC_PATH}/include/*.h)
SOURCE_GROUP("Source Files" FILES ${FOLDER_SOURCE})
SOURCE_GROUP("Header Files" FILES ${FOLDER_HEADER})


INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include
                    ${YARP_INCLUDE_DIRS}
                    ${ICUB_INCLUDE_DIRS})


ADD_EXECUTABLE(${MODULENAME} ${FOLDER_SOURCE} ${FOLDER_HEADER})
TARGET_LINK_LIBRARIES( ${MODULENAME} ${YARP_LIBRARIES} ${ICUB_LIBRARIES} )


# # Set bin paths
# set(LIBRARY_OUTPUT_PATH ${MY_SRC_PATH}/lib)
# message(STATUS "Libraries will be placed in ${LIBRARY_OUTPUT_PATH}")
# set(EXECUTABLE_OUTPUT_PATH ${MY_SRC_PATH}/bin)
# message(STATUS "Executables will be placed in ${EXECUTABLE_OUTPUT_PATH}")
# # this doesn't happen automatically for makefiles
# make_directory(${LIBRARY_OUTPUT_PATH})
# make_directory(${EXECUTABLE_OUTPUT_PATH})


yarp_install(TARGETS ${MODULENAME} DESTINATION bin)
#INSTALL(TARGETS ${PROJECTNAME} DESTINATION bin)
