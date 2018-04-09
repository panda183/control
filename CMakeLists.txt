cmake_minimum_required(VERSION 2.8)
project( Client )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set( OPENNI2_INCLUDE /home/dung/Downloads/OpenNI_2.3.0.43/Linux/OpenNI-Linux-x64-2.3/Include)
set( OPENNI2_REDIST  /home/dung/Downloads/OpenNI_2.3.0.43/Linux/OpenNI-Linux-x64-2.3/Redist)
include_directories( ${OPENNI2_INCLUDE} )
link_directories ( ${OPENNI2_REDIST} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable( Client
    client.cpp
    LaneDetector/LaneDetector.cpp
    Driver/Driver.cpp
    SignDetector/SignDetector.cpp
    Utilities/Utilities.cpp
)

target_link_libraries( Client ${OpenCV_LIBS} OpenNI2 )
