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
    /home/Gene/ffmpeg-linux-6.0/include
)

#lib path
link_directories(/home/Gene/ffmpeg-linux-6.0/lib)

# libraries
#set(CMAKE_IGNORE_PATH /usr/lib /usr/local/lib)

target_link_libraries(stream_program PRIVATE
/home/Gene/ffmpeg-linux-6.0/lib/libavformat.a
/home/Gene/ffmpeg-linux-6.0/lib/libavcodec.a
/home/Gene/ffmpeg-linux-6.0/lib/libavutil.a
/home/Gene/ffmpeg-linux-6.0/lib/libvpx.a
/home/Gene/ffmpeg-linux-6.0/lib/libswscale.a
/home/Gene/ffmpeg-linux-6.0/lib/libswresample.a
pthread
m
z
)

message("Library path: ${CMAKE_SOURCE_DIR}/lib")
