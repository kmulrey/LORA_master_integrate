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
include CMakeFiles/lofar.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/lofar.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lofar.dir/flags.make

CMakeFiles/lofar.dir/src/LOFAR_Communications.cxx.o: CMakeFiles/lofar.dir/flags.make
CMakeFiles/lofar.dir/src/LOFAR_Communications.cxx.o: ../src/LOFAR_Communications.cxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/lofar.dir/src/LOFAR_Communications.cxx.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/lofar.dir/src/LOFAR_Communications.cxx.o -c /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/src/LOFAR_Communications.cxx

CMakeFiles/lofar.dir/src/LOFAR_Communications.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lofar.dir/src/LOFAR_Communications.cxx.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/src/LOFAR_Communications.cxx > CMakeFiles/lofar.dir/src/LOFAR_Communications.cxx.i

CMakeFiles/lofar.dir/src/LOFAR_Communications.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lofar.dir/src/LOFAR_Communications.cxx.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/src/LOFAR_Communications.cxx -o CMakeFiles/lofar.dir/src/LOFAR_Communications.cxx.s

# Object files for target lofar
lofar_OBJECTS = \
"CMakeFiles/lofar.dir/src/LOFAR_Communications.cxx.o"

# External object files for target lofar
lofar_EXTERNAL_OBJECTS =

liblofar.a: CMakeFiles/lofar.dir/src/LOFAR_Communications.cxx.o
liblofar.a: CMakeFiles/lofar.dir/build.make
liblofar.a: CMakeFiles/lofar.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library liblofar.a"
	$(CMAKE_COMMAND) -P CMakeFiles/lofar.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lofar.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lofar.dir/build: liblofar.a

.PHONY : CMakeFiles/lofar.dir/build

CMakeFiles/lofar.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lofar.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lofar.dir/clean

CMakeFiles/lofar.dir/depend:
	cd /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build/CMakeFiles/lofar.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/lofar.dir/depend

