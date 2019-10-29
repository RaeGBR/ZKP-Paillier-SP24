cmake_minimum_required(VERSION 3.4.1)
project(cryptoplus)
# set(CMAKE_C_COMPILER "gcc")
# set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../build/jni/cryptoplus)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../build/jni/cryptoplus)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../build/jni/cryptoplus)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  # set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  ADD_DEFINITIONS(
      -std=c++11 # Or -std=c++0x
      # Other flags
  )
endif()

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
include(UseJava)

include_directories(
        ${JNI_INCLUDE_DIRS}
        ../../deps/djinni/support-lib/
        ../../deps/djinni/support-lib/jni/
        ../../generated-src/cpp/
        ../../generated-src/jni/
        ../../deps/cryptopp/
        ../../src/cpp/
)

file(GLOB cryptoplus_sources
        ../../deps/djinni/support-lib/jni/*.cpp
        ../../generated-src/jni/*.cpp
        ../../src/cpp/lib/*.cpp
        ../../src/cpp/lib/**/*.cpp
)

# static 64
add_library(cryptoplus SHARED ${cryptoplus_sources})
target_link_libraries(
        cryptoplus
        ${CMAKE_SOURCE_DIR}/../../deps/cryptopp/x64/Output/Release/cryptlib.lib
)
