# Crypto Library

## Dependencies

- CMake
- g++ combiler
- lcov (C++ coverage testing tool)
- NTL 11.4.3
- JDK1.8
- Gradle >4.7
- NodeJS >8.0.0
- emscripten
- Android Studio

## Git Submodule

- Crypto++
- Google Test
- Djinni

You need to update the submodule after project clone.

```
git submodule update --init --recursive
```

## Build Commands

```bash
# full rebuild for C++ project
make

# build dependencies and submodules
make deps_build

# build core library and run test case
make lib_build

# build and package for iOS development
make ios_build

# build and package for Android development
make android_build

# build and package for JS development
make wasm_build
```

## Troubleshoot

### ios.toolchain.cmake error

Error:

```
CMake Error at cmake/ios.toolchain.cmake:145 (message):
  Invalid CMAKE_OSX_SYSROOT: does not exist.
```

It may cause by some updates (OS/tools) are not synchronize with XCode, please try command:

```
sudo xcode-select -s /Applications/Xcode.app/Contents/Developer
```

## Windows Tips

### Linux CLI Tools

You may use `scoop` to install the following useful tools:

- cmake
- gcc
- gradle
- make

You can install `rsync` through `MinGW`, then run the following command to port the binary to Git Bash.

```
echo '/c/MinGW/msys/1.0/bin/rsync "$@"' >/usr/bin/rsync
chmod +x /usr/bin/rsync
```

And you should install `clang` / `llvm`.
