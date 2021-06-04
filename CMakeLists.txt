cmake_minimum_required(VERSION 3.13)
project(zmq-data-stream-cpp)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(zmq_data_streaming SHARED
        src/DataServiceWriter.cpp
        src/DataServiceReader.cpp
        )

target_include_directories(zmq_data_streaming PUBLIC
        src/)

target_link_libraries(zmq_data_streaming zmq)

add_executable(testApp src/Hello.cpp)

target_include_directories(testApp PUBLIC
        src/)

target_link_libraries(testApp
        zmq_data_streaming)

enable_testing()

add_executable(zmq_framework_test
        test/DataServiceWriterTest.cpp
        test/DataServiceReaderTest.cpp)

target_include_directories(zmq_framework_test PUBLIC
        src/
        test/)

target_link_libraries(zmq_framework_test
        zmq_data_streaming
        gtest_main
        )

include(GoogleTest)
gtest_discover_tests(zmq_framework_test)