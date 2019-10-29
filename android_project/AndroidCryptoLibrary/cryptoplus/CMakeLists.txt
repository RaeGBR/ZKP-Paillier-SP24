# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

include(AndroidNdkModules)
android_ndk_import_module_cpufeatures()

set(CRYPTOPP_BUILD_DIR ${CMAKE_SOURCE_DIR}/../../cryptopp)
add_library(cryptopp SHARED IMPORTED)
set_target_properties(
        cryptopp
        PROPERTIES IMPORTED_LOCATION
        ${CRYPTOPP_BUILD_DIR}/libs/${ANDROID_ABI}/libcryptopp_shared.so
)

file(GLOB cryptoplus_sources
        ../../../deps/djinni/support-lib/jni/*.cpp
        ../../../generated-src/jni/*.cpp
        ../../../src/cpp/lib/*.cpp
        ../../../src/cpp/lib/**/*.cpp
)
add_library(cryptoplus SHARED ${cryptoplus_sources})

# Specifies a path to native header files.
include_directories(
        ../../../deps/djinni/support-lib/
        ../../../deps/djinni/support-lib/jni/
        ../../../generated-src/cpp/
        ../../../generated-src/jni/
        ../../../deps/cryptopp/
        ../../../src/cpp/
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.


target_link_libraries(
        cryptoplus
        cpufeatures cryptopp
)