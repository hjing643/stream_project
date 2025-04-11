#Make minimu version
cmake_minimum_required(VERSION 2.8)

#set project name
project(stream_project)

# set c++ standard
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)    
#set(CMAKE_CXX_EXTENSIONS OFF)  

# set static
set(CMAKE_EXE_LINKER_FLAGS "-static")

#set debug
set(CMAKE_BUILD_TYPE Debug)

#set release + debug infor
#set(CMAKE_BUILD_TYPE RelWithDebInfo)
#add source file, cpp file add to parameter SOURCES
file(GLOB_RECURSE SOURCES "src/*.cpp")

# executable file
add_executable(stream_program ${SOURCES})

# header folder
target_include_directories(stream_program PRIVATE 
    ${CMAKE_SOURCE_DIR}/include)

#lib path
link_directories(${CMAKE_SOURCE_DIR}/lib)

# libraries
target_link_libraries(stream_program PRIVATE 
    avcodec
    avformat
    avutil
    swscale)
