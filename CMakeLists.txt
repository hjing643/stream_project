#Make minimu version
cmake_minimum_required(VERSION 2.8)

#set project name
project(stream_project)

# set c++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)   
#set(CMAKE_CXX_EXTENSIONS OFF)  

# set static
#set(CMAKE_EXE_LINKER_FLAGS "-static")
set(MY_HOME_DIR $ENV{HOME})

#set debug
set(CMAKE_BUILD_TYPE Debug)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

set(CMAKE_IGNORE_PATH "/usr/include" "/usr/local/include" "/usr/local/lib")

#set release + debug infor
#set(CMAKE_BUILD_TYPE RelWithDebInfo)
#add source file, cpp file add to parameter SOURCES
file(GLOB_RECURSE SOURCES "src/*.cpp")

# executable file
add_executable(stream_program ${SOURCES})


# header folder
target_include_directories(stream_program BEFORE PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${MY_HOME_DIR}/third-party/ffmpeg6.0/linux_build/include
)

#lib path

# libraries
#set(CMAKE_IGNORE_PATH /usr/lib /usr/local/lib)

target_link_libraries(stream_program PRIVATE
    ${MY_HOME_DIR}/third-party/ffmpeg6.0/linux_build/lib/libavfilter.a
    ${MY_HOME_DIR}/third-party/ffmpeg6.0/linux_build/lib/libpostproc.a
    ${MY_HOME_DIR}/third-party/ffmpeg6.0/linux_build/lib/libswresample.a
    ${MY_HOME_DIR}/third-party/ffmpeg6.0/linux_build/lib/libswscale.a
    ${MY_HOME_DIR}/third-party/ffmpeg6.0/linux_build/lib/libavformat.a
    ${MY_HOME_DIR}/third-party/ffmpeg6.0/linux_build/lib/libavcodec.a
    ${MY_HOME_DIR}/third-party/ffmpeg6.0/linux_build/lib/libx264.a
    ${MY_HOME_DIR}/third-party/ffmpeg6.0/linux_build/lib/libx265.a
    ${MY_HOME_DIR}/third-party/ffmpeg6.0/linux_build/lib/libvpx.a
    ${MY_HOME_DIR}/third-party/ffmpeg6.0/linux_build/lib/libfdk-aac.a
    ${MY_HOME_DIR}/third-party/ffmpeg6.0/linux_build/lib/libavutil.a
    pthread
    m
    z
)

message("Library path: ${CMAKE_SOURCE_DIR}/lib")
