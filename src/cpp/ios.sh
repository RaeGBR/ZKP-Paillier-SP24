#!/usr/bin/env bash

# Helper Functions
function join {
  for s in ${$2[@]}; do
    echo $s
  done
}

# Configure Terminal Color
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
NC='\033[0m'

# Configure Basic Directory Info
ROOT_DIR=../..
SRC_DIR=.
PROJ_NAME=libcryptoplus
OUT_DIR=$ROOT_DIR/build
CRYPTOPP_DIR=$ROOT_DIR/deps/cryptopp
STATIC_LIB=$ROOT_DIR/deps/cryptopp/libcryptopp.a
DJINNI_GEN_DIR=$ROOT_DIR/generated-src/cpp
CPP_OUT=$OUT_DIR/ios/cryptoplus

# Configure Compile Flags
FLAGS="-stdlib=libc++ -DNDEBUG -g2 -O3 -fPIC -pipe -Wall -Wno-everything"

# Configure Output Target
CPP_TARGET=$CPP_OUT/$PROJ_NAME

# Configure IPHONE OS Architectures
ARCH_IPHONE=(armv7 armv7s arm64)

# Configure Simulator Architectures
ARCH_SIMULATOR=(i386 x86_64)

# Configure Source Files
LIB_SRC_FILES=$(find ./lib -name "*.cpp")
LIB_LIST=""
for src in ${LIB_SRC_FILES[@]}; do
  LIB_LIST+=$(echo "$src" | sed 's/^.\/lib\/\(.*\)\.cpp$/\1/g')
  LIB_LIST+=" "
done

# Configure Output Files
LIB_OUT=""
for src in ${LIB_LIST[@]}; do
  LIB_OUT+="$SRC_DIR/lib/$src.o "
done

ARCHIVES=""
for out in ${ARCH_IPHONE[@]}; do
  ARCHIVES+="$CPP_OUT/$out/$PROJ_NAME.a "
done
for out in ${ARCH_SIMULATOR[@]}; do
  ARCHIVES+="$CPP_OUT/$out/$PROJ_NAME.a "
done


##### Start Compile Source Files in iPhone OS #####

# Create Output Directory
echo -e ${CYAN}Create Output Directory: ${NC} $CPP_OUT
mkdir -p $CPP_OUT

for arch in ${ARCH_IPHONE[@]}; do
  echo -e  ${Yellow} Configure Architecture Environment: $arch ${NC}
  source $SRC_DIR/setenv-ios.sh iphoneos $arch

  for lib in ${LIB_LIST[@]}; do
    # Configure CPP Source Include Files
    INC="-I$CRYPTOPP_DIR -I$DJINNI_GEN_DIR -I$SRC_DIR"

    # Compile .cpp into .o
    clang++ $INC $FLAGS $IOS_FLAGS -isysroot $IOS_SYSROOT -arch $arch -c $SRC_DIR/lib/$lib.cpp -o $SRC_DIR/lib/$lib.o -std=c++11
  done

  # Create Architecture Direcrory & Archive
  mkdir -p $CPP_OUT/$arch
  libtool -static -o $CPP_OUT/$arch/$PROJ_NAME.a $LIB_OUT
done

##### Start Compile Source Files in Simulator #####
make clean
for arch in ${ARCH_SIMULATOR[@]}; do
  echo -e  ${Yellow} Configure Architecture Environment: $arch ${NC}
  source $SRC_DIR/setenv-ios.sh iphonesimulator $arch

  for lib in ${LIB_LIST[@]}; do
    # Configure CPP Source Include Files
    INC="-I$CRYPTOPP_DIR -I$DJINNI_GEN_DIR -I$SRC_DIR"

    # Compile .cpp into .o
    clang++ $INC $FLAGS $IOS_FLAGS -isysroot $IOS_SYSROOT -arch $arch -c $SRC_DIR/lib/$lib.cpp -o $SRC_DIR/lib/$lib.o -std=c++11
  done

  # Create Architecture Direcrory & Archive
  mkdir -p $CPP_OUT/$arch
  libtool -static -o $CPP_OUT/$arch/$PROJ_NAME.a $LIB_OUT

  # Clean All Architecture Output Files
  make clean
done

# Create Fat Library
make clean
lipo -create $ARCHIVES -output $CPP_OUT/$PROJ_NAME.a

# Verify the four architectures are present
xcrun -sdk iphoneos lipo -info $CPP_OUT/$PROJ_NAME.a

