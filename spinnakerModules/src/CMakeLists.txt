# Copyright: (C) 2017 University of Plymouth Centre for Robotics and Neural Systems (CRNS)
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.6)

option(ENABLE_eventSender "Build events spike sender module" OFF)
option(ENABLE_spikesSender "Build vision spike sender module" OFF)
option(ENABLE_lipReceiver "Build lip spikes receiver module" OFF)


IF(ENABLE_eventSender)
  add_subdirectory(spinnaker_event_spikes_sender)
ENDIF(ENABLE_eventSender)

IF(ENABLE_spikesSender)
  add_subdirectory(spinnaker_spikes_sender)
ENDIF(ENABLE_spikesSender)

IF(ENABLE_lipReceiver)
  add_subdirectory(spinnaker_lip_spikes_receiver)
ENDIF(ENABLE_lipReceiver)
