# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.13.3/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.13.3/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build

# Include any dependencies generated for this target.
include CMakeFiles/main_muon_calib.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/main_muon_calib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/main_muon_calib.dir/flags.make

CMakeFiles/main_muon_calib.dir/src/main_muon_calib.cxx.o: CMakeFiles/main_muon_calib.dir/flags.make
CMakeFiles/main_muon_calib.dir/src/main_muon_calib.cxx.o: ../src/main_muon_calib.cxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/main_muon_calib.dir/src/main_muon_calib.cxx.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main_muon_calib.dir/src/main_muon_calib.cxx.o -c /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/src/main_muon_calib.cxx

CMakeFiles/main_muon_calib.dir/src/main_muon_calib.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main_muon_calib.dir/src/main_muon_calib.cxx.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/src/main_muon_calib.cxx > CMakeFiles/main_muon_calib.dir/src/main_muon_calib.cxx.i

CMakeFiles/main_muon_calib.dir/src/main_muon_calib.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main_muon_calib.dir/src/main_muon_calib.cxx.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/src/main_muon_calib.cxx -o CMakeFiles/main_muon_calib.dir/src/main_muon_calib.cxx.s

# Object files for target main_muon_calib
main_muon_calib_OBJECTS = \
"CMakeFiles/main_muon_calib.dir/src/main_muon_calib.cxx.o"

# External object files for target main_muon_calib
main_muon_calib_EXTERNAL_OBJECTS =

main_muon_calib: CMakeFiles/main_muon_calib.dir/src/main_muon_calib.cxx.o
main_muon_calib: CMakeFiles/main_muon_calib.dir/build.make
main_muon_calib: liboperations.a
main_muon_calib: liblora.dylib
main_muon_calib: libsocket_calls.a
main_muon_calib: libbuffer.a
main_muon_calib: libstructs.a
main_muon_calib: libfunctionslib.a
main_muon_calib: liblofar.a
main_muon_calib: /usr/local/root/lib/libCore.so
main_muon_calib: /usr/local/root/lib/libImt.so
main_muon_calib: /usr/local/root/lib/libRIO.so
main_muon_calib: /usr/local/root/lib/libNet.so
main_muon_calib: /usr/local/root/lib/libHist.so
main_muon_calib: /usr/local/root/lib/libGraf.so
main_muon_calib: /usr/local/root/lib/libGraf3d.so
main_muon_calib: /usr/local/root/lib/libGpad.so
main_muon_calib: /usr/local/root/lib/libROOTDataFrame.so
main_muon_calib: /usr/local/root/lib/libTree.so
main_muon_calib: /usr/local/root/lib/libTreePlayer.so
main_muon_calib: /usr/local/root/lib/libRint.so
main_muon_calib: /usr/local/root/lib/libPostscript.so
main_muon_calib: /usr/local/root/lib/libMatrix.so
main_muon_calib: /usr/local/root/lib/libPhysics.so
main_muon_calib: /usr/local/root/lib/libMathCore.so
main_muon_calib: /usr/local/root/lib/libThread.so
main_muon_calib: /usr/local/root/lib/libMultiProc.so
main_muon_calib: CMakeFiles/main_muon_calib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable main_muon_calib"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main_muon_calib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/main_muon_calib.dir/build: main_muon_calib

.PHONY : CMakeFiles/main_muon_calib.dir/build

CMakeFiles/main_muon_calib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/main_muon_calib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/main_muon_calib.dir/clean

CMakeFiles/main_muon_calib.dir/depend:
	cd /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build/CMakeFiles/main_muon_calib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/main_muon_calib.dir/depend

