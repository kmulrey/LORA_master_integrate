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
include CMakeFiles/structs.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/structs.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/structs.dir/flags.make

CMakeFiles/structs.dir/src/Structs.cxx.o: CMakeFiles/structs.dir/flags.make
CMakeFiles/structs.dir/src/Structs.cxx.o: ../src/Structs.cxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/structs.dir/src/Structs.cxx.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/structs.dir/src/Structs.cxx.o -c /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/src/Structs.cxx

CMakeFiles/structs.dir/src/Structs.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/structs.dir/src/Structs.cxx.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/src/Structs.cxx > CMakeFiles/structs.dir/src/Structs.cxx.i

CMakeFiles/structs.dir/src/Structs.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/structs.dir/src/Structs.cxx.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/src/Structs.cxx -o CMakeFiles/structs.dir/src/Structs.cxx.s

# Object files for target structs
structs_OBJECTS = \
"CMakeFiles/structs.dir/src/Structs.cxx.o"

# External object files for target structs
structs_EXTERNAL_OBJECTS =

libstructs.a: CMakeFiles/structs.dir/src/Structs.cxx.o
libstructs.a: CMakeFiles/structs.dir/build.make
libstructs.a: CMakeFiles/structs.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libstructs.a"
	$(CMAKE_COMMAND) -P CMakeFiles/structs.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/structs.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/structs.dir/build: libstructs.a

.PHONY : CMakeFiles/structs.dir/build

CMakeFiles/structs.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/structs.dir/cmake_clean.cmake
.PHONY : CMakeFiles/structs.dir/clean

CMakeFiles/structs.dir/depend:
	cd /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build /Users/kmulrey/LOFAR/LORA/DAQ_test/DAQ_test/lora_daq-master/build/CMakeFiles/structs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/structs.dir/depend
