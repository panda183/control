cmake_minimum_required(VERSION 2.8)
project( Client )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set( OPENNI2_INCLUDE /home/dung/Downloads/OpenNI_2.3.0.43/Linux/OpenNI-Linux-x64-2.3/Include)
set( OPENNI2_REDIST  /home/dung/Downloads/OpenNI_2.3.0.43/Linux/OpenNI-Linux-x64-2.3/Redist)
include_directories( ${OPENNI2_INCLUDE} )
link_directories ( ${OPENNI2_REDIST} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

add_executable( Client
    src/client.cpp
    src/LaneDetector.cpp
    src/Driver.cpp
    src/SignDetector.cpp
    src/Utilities.cpp
)
add_executable( Setup
    src/setup.cpp
    src/Utilities.cpp
)

target_link_libraries( Client ${OpenCV_LIBS} OpenNI2 )
target_link_libraries( Setup ${OpenCV_LIBS} OpenNI2 )
