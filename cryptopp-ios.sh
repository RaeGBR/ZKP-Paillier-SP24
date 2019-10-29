#!/bin/bash

# Go into the Crypto++ directory
pushd ./deps/cryptopp > /dev/null

MAKE="make -f GNUmakefile-cross"
OUT_DIR=./build/ios/

ARCHS_OUT=''
# ARCHS='ARMv7s ARM64'
ARCHS='ARMv7 ARMv7s ARM64 i386 x86_64'
for ARCH in $ARCHS
do
  MACHINE='iphonesimulator'
  if [[ $ARCH == ARM* ]]; then
    MACHINE='iphoneos'
  fi
  echo "****************************************"
  echo '* Build' $MACHINE $ARCH;
  echo "****************************************"
  . ./setenv-ios.sh $MACHINE $ARCH
  $MAKE clean
  if [[ $ARCH == ARM64* ]]; then
    CXXFLAGS="-DCRYPTOPP_BOOL_ARM_CRC32_INTRINSICS_AVAILABLE=0" $MAKE static
  else
    $MAKE static
  fi
  ARCH2=`echo "$ARCH" | tr '[:upper:]' '[:lower:]'`
  ARCH_OUT_DIR=$OUT_DIR/$ARCH2
  mkdir -p $ARCH_OUT_DIR;
  cp libcryptopp.a $ARCH_OUT_DIR/libcryptopp.a
  ARCHS_OUT="$ARCHS_OUT $ARCH_OUT_DIR/libcryptopp.a"
done

# Sixth, create the fat library
echo "****************************************"
echo "* Build Fat Library"
echo "****************************************"
$MAKE clean
lipo -create $ARCHS_OUT -output ./libcryptopp.a

# Seventh, verify the four architectures are present
echo "****************************************"
echo "* Verify built architecutres"
echo "****************************************"
xcrun -sdk iphoneos lipo -info libcryptopp.a

# Eighth, remove unneeded artifacts
echo "****************************************"
rm *.so *.exe *.dylib

# Return to previous directory
popd > /dev/null
