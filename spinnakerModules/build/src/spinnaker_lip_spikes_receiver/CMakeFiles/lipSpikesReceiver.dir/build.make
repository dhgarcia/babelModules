# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/daniel/Desktop/BBC/spinnakerModules

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/daniel/Desktop/BBC/spinnakerModules/build

# Include any dependencies generated for this target.
include src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/depend.make

# Include the progress variables for this target.
include src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/progress.make

# Include the compile flags for this target's objects.
include src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/flags.make

src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o: src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/flags.make
src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o: ../src/spinnaker_lip_spikes_receiver/src/spinnakerIOinterface.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/daniel/Desktop/BBC/spinnakerModules/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o"
	cd /home/daniel/Desktop/BBC/spinnakerModules/build/src/spinnaker_lip_spikes_receiver && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o -c /home/daniel/Desktop/BBC/spinnakerModules/src/spinnaker_lip_spikes_receiver/src/spinnakerIOinterface.cpp

src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.i"
	cd /home/daniel/Desktop/BBC/spinnakerModules/build/src/spinnaker_lip_spikes_receiver && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/daniel/Desktop/BBC/spinnakerModules/src/spinnaker_lip_spikes_receiver/src/spinnakerIOinterface.cpp > CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.i

src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.s"
	cd /home/daniel/Desktop/BBC/spinnakerModules/build/src/spinnaker_lip_spikes_receiver && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/daniel/Desktop/BBC/spinnakerModules/src/spinnaker_lip_spikes_receiver/src/spinnakerIOinterface.cpp -o CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.s

src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o.requires:

.PHONY : src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o.requires

src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o.provides: src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o.requires
	$(MAKE) -f src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/build.make src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o.provides.build
.PHONY : src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o.provides

src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o.provides.build: src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o


src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o: src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/flags.make
src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o: ../src/spinnaker_lip_spikes_receiver/src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/daniel/Desktop/BBC/spinnakerModules/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o"
	cd /home/daniel/Desktop/BBC/spinnakerModules/build/src/spinnaker_lip_spikes_receiver && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o -c /home/daniel/Desktop/BBC/spinnakerModules/src/spinnaker_lip_spikes_receiver/src/main.cpp

src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.i"
	cd /home/daniel/Desktop/BBC/spinnakerModules/build/src/spinnaker_lip_spikes_receiver && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/daniel/Desktop/BBC/spinnakerModules/src/spinnaker_lip_spikes_receiver/src/main.cpp > CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.i

src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.s"
	cd /home/daniel/Desktop/BBC/spinnakerModules/build/src/spinnaker_lip_spikes_receiver && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/daniel/Desktop/BBC/spinnakerModules/src/spinnaker_lip_spikes_receiver/src/main.cpp -o CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.s

src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o.requires:

.PHONY : src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o.requires

src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o.provides: src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o.requires
	$(MAKE) -f src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/build.make src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o.provides.build
.PHONY : src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o.provides

src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o.provides.build: src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o


# Object files for target lipSpikesReceiver
lipSpikesReceiver_OBJECTS = \
"CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o" \
"CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o"

# External object files for target lipSpikesReceiver
lipSpikesReceiver_EXTERNAL_OBJECTS =

bin/lipSpikesReceiver: src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o
bin/lipSpikesReceiver: src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o
bin/lipSpikesReceiver: src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/build.make
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_videostab.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_ts.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_superres.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_stitching.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_ocl.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_gpu.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_contrib.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libYARP_gsl.so.2.3.68
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libYARP_dev.so.2.3.68
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libYARP_name.so.2.3.68
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libYARP_init.so.2.3.68
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_photo.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_legacy.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_video.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_objdetect.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_ml.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_calib3d.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_features2d.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_flann.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libopencv_core.so.2.4.9
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libYARP_math.so.2.3.68
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libYARP_sig.so.2.3.68
bin/lipSpikesReceiver: /usr/lib/x86_64-linux-gnu/libYARP_OS.so.2.3.68
bin/lipSpikesReceiver: src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/daniel/Desktop/BBC/spinnakerModules/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../../bin/lipSpikesReceiver"
	cd /home/daniel/Desktop/BBC/spinnakerModules/build/src/spinnaker_lip_spikes_receiver && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lipSpikesReceiver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/build: bin/lipSpikesReceiver

.PHONY : src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/build

src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/requires: src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/spinnakerIOinterface.cpp.o.requires
src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/requires: src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/src/main.cpp.o.requires

.PHONY : src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/requires

src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/clean:
	cd /home/daniel/Desktop/BBC/spinnakerModules/build/src/spinnaker_lip_spikes_receiver && $(CMAKE_COMMAND) -P CMakeFiles/lipSpikesReceiver.dir/cmake_clean.cmake
.PHONY : src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/clean

src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/depend:
	cd /home/daniel/Desktop/BBC/spinnakerModules/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/daniel/Desktop/BBC/spinnakerModules /home/daniel/Desktop/BBC/spinnakerModules/src/spinnaker_lip_spikes_receiver /home/daniel/Desktop/BBC/spinnakerModules/build /home/daniel/Desktop/BBC/spinnakerModules/build/src/spinnaker_lip_spikes_receiver /home/daniel/Desktop/BBC/spinnakerModules/build/src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/spinnaker_lip_spikes_receiver/CMakeFiles/lipSpikesReceiver.dir/depend
