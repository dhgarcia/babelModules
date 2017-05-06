# Copyright: (C) 2017 University of Plymouth Centre for Robotics and Neural Systems (CRNS)
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.6)

option(ENABLE_lipGazeCtrl "Build lipGazeCtrl module" OFF)
option(ENABLE_namingModule "Build namingModule module" OFF)
#option(ENABLE_speechProduction_acapela "Build speechProduction_with_Acapela module" OFF)

# modules that do not need OpenCV
IF(ENABLE_lipGazeCtrl)
  add_subdirectory(lipGazeCtrl)
ENDIF(ENABLE_lipGazeCtrl)

IF(ENABLE_namingModule)
  add_subdirectory(namingModule)
ENDIF(ENABLE_namingModule)

#IF(ENABLE_speechProduction_acapela)
#  add_subdirectory(speechProduction_with_Acapela)
#ENDIF(ENABLE_speechProduction_acapela)

# find_package(OpenCV)
# IF(OPENCV_FOUND)
#   option(ENABLE_basicVisionStuff "Build basicVisionStuff_with_OpenCV module" OFF)
#   IF(ENABLE_basicVisionStuff)
#     add_subdirectory(basicVisionStuff_with_OpenCV)
#   ENDIF(ENABLE_basicVisionStuff)
# ELSE(OPENCV_FOUND)
#   message("Warning: OpenCV not found. Skipping OpenCV dependent stuff")
# ENDIF(OPENCV_FOUND)
