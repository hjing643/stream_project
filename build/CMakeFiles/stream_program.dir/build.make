# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/Gene/git/stream_project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/Gene/git/stream_project/build

# Include any dependencies generated for this target.
include CMakeFiles/stream_program.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/stream_program.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/stream_program.dir/flags.make

CMakeFiles/stream_program.dir/src/code/main.cpp.o: CMakeFiles/stream_program.dir/flags.make
CMakeFiles/stream_program.dir/src/code/main.cpp.o: ../src/code/main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/Gene/git/stream_project/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/stream_program.dir/src/code/main.cpp.o"
	/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/stream_program.dir/src/code/main.cpp.o -c /home/Gene/git/stream_project/src/code/main.cpp

CMakeFiles/stream_program.dir/src/code/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/stream_program.dir/src/code/main.cpp.i"
	/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/Gene/git/stream_project/src/code/main.cpp > CMakeFiles/stream_program.dir/src/code/main.cpp.i

CMakeFiles/stream_program.dir/src/code/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/stream_program.dir/src/code/main.cpp.s"
	/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/Gene/git/stream_project/src/code/main.cpp -o CMakeFiles/stream_program.dir/src/code/main.cpp.s

CMakeFiles/stream_program.dir/src/code/main.cpp.o.requires:
.PHONY : CMakeFiles/stream_program.dir/src/code/main.cpp.o.requires

CMakeFiles/stream_program.dir/src/code/main.cpp.o.provides: CMakeFiles/stream_program.dir/src/code/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/stream_program.dir/build.make CMakeFiles/stream_program.dir/src/code/main.cpp.o.provides.build
.PHONY : CMakeFiles/stream_program.dir/src/code/main.cpp.o.provides

CMakeFiles/stream_program.dir/src/code/main.cpp.o.provides.build: CMakeFiles/stream_program.dir/src/code/main.cpp.o

# Object files for target stream_program
stream_program_OBJECTS = \
"CMakeFiles/stream_program.dir/src/code/main.cpp.o"

# External object files for target stream_program
stream_program_EXTERNAL_OBJECTS =

stream_program: CMakeFiles/stream_program.dir/src/code/main.cpp.o
stream_program: CMakeFiles/stream_program.dir/build.make
stream_program: CMakeFiles/stream_program.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable stream_program"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/stream_program.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/stream_program.dir/build: stream_program
.PHONY : CMakeFiles/stream_program.dir/build

CMakeFiles/stream_program.dir/requires: CMakeFiles/stream_program.dir/src/code/main.cpp.o.requires
.PHONY : CMakeFiles/stream_program.dir/requires

CMakeFiles/stream_program.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/stream_program.dir/cmake_clean.cmake
.PHONY : CMakeFiles/stream_program.dir/clean

CMakeFiles/stream_program.dir/depend:
	cd /home/Gene/git/stream_project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/Gene/git/stream_project /home/Gene/git/stream_project /home/Gene/git/stream_project/build /home/Gene/git/stream_project/build /home/Gene/git/stream_project/build/CMakeFiles/stream_program.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/stream_program.dir/depend

