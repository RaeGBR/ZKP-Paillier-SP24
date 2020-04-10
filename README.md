# Crypto Library

## Dependencies

- CMake
- g++ combiler
- lcov (C++ coverage testing tool)
- [NTL 11.4.3](https://www.shoup.net/ntl/)

In mac, you can install NTL through Homebrew

```
brew install ntl
```

## Git Submodule

- Google Test

You need to update the submodule after project clone.

```
git submodule update --init --recursive
```

## Build Commands

```bash
# full rebuild for C++ project
make

# build submodules
make deps_build

# build core library and run test case
make lib_build
```

## Troubleshoot

## Windows Tips

### Linux CLI Tools

You may use `scoop` to install the following useful tools:

- cmake
- gcc
- make

You can install `rsync` through `MinGW`, then run the following command to port the binary to Git Bash.

```
echo '/c/MinGW/msys/1.0/bin/rsync "$@"' >/usr/bin/rsync
chmod +x /usr/bin/rsync
```

And you should install `clang` / `llvm`.
