cmake_minimum_required(VERSION 3.13)
project(zmq-data-stream-cpp)

set(CMAKE_CXX_STANDARD 14)
add_executable(testApp Hello.cpp
        DataServiceWriter.cpp)

target_link_libraries(testApp zmq)