# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/nixiaowei/Desktop/msgpacktest

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nixiaowei/Desktop/msgpacktest/build

# Include any dependencies generated for this target.
include CMakeFiles/msgpacktest.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/msgpacktest.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/msgpacktest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/msgpacktest.dir/flags.make

CMakeFiles/msgpacktest.dir/msgpacktest.cpp.o: CMakeFiles/msgpacktest.dir/flags.make
CMakeFiles/msgpacktest.dir/msgpacktest.cpp.o: ../msgpacktest.cpp
CMakeFiles/msgpacktest.dir/msgpacktest.cpp.o: CMakeFiles/msgpacktest.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nixiaowei/Desktop/msgpacktest/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/msgpacktest.dir/msgpacktest.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/msgpacktest.dir/msgpacktest.cpp.o -MF CMakeFiles/msgpacktest.dir/msgpacktest.cpp.o.d -o CMakeFiles/msgpacktest.dir/msgpacktest.cpp.o -c /home/nixiaowei/Desktop/msgpacktest/msgpacktest.cpp

CMakeFiles/msgpacktest.dir/msgpacktest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/msgpacktest.dir/msgpacktest.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nixiaowei/Desktop/msgpacktest/msgpacktest.cpp > CMakeFiles/msgpacktest.dir/msgpacktest.cpp.i

CMakeFiles/msgpacktest.dir/msgpacktest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/msgpacktest.dir/msgpacktest.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nixiaowei/Desktop/msgpacktest/msgpacktest.cpp -o CMakeFiles/msgpacktest.dir/msgpacktest.cpp.s

# Object files for target msgpacktest
msgpacktest_OBJECTS = \
"CMakeFiles/msgpacktest.dir/msgpacktest.cpp.o"

# External object files for target msgpacktest
msgpacktest_EXTERNAL_OBJECTS =

msgpacktest: CMakeFiles/msgpacktest.dir/msgpacktest.cpp.o
msgpacktest: CMakeFiles/msgpacktest.dir/build.make
msgpacktest: CMakeFiles/msgpacktest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/nixiaowei/Desktop/msgpacktest/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable msgpacktest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/msgpacktest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/msgpacktest.dir/build: msgpacktest
.PHONY : CMakeFiles/msgpacktest.dir/build

CMakeFiles/msgpacktest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/msgpacktest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/msgpacktest.dir/clean

CMakeFiles/msgpacktest.dir/depend:
	cd /home/nixiaowei/Desktop/msgpacktest/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nixiaowei/Desktop/msgpacktest /home/nixiaowei/Desktop/msgpacktest /home/nixiaowei/Desktop/msgpacktest/build /home/nixiaowei/Desktop/msgpacktest/build /home/nixiaowei/Desktop/msgpacktest/build/CMakeFiles/msgpacktest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/msgpacktest.dir/depend
