# Crypto Library

## Dependencies

- Build tools
  - CMake
  - g++ compiler
  - lcov (C++ coverage testing tool)
- Library dependencies
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

## How to use

### Generate Paillier Encryption Key

```c++
// Generate new keys
size_t byteLength = 128;
auto crypto = make_shared<PaillierEncryption>(byteLength);
auto sk1 = crypto->getPrivateElement1(); // private key component
auto sk2 = crypto->getPrivateElement2(); // private key component
auto pk = crypto->getPublicKey();        // public key
auto GP_Q = crypto->getGroupQ();         // public parameter: group element Q
auto GP_P = crypto->getGroupP();         // public parameter: group order p
auto GP_G = crypto->getGroupG();         // public parameter: group generator g
auto GP_Gi = crypto->genGenerators(10);  // public parameter: more generators if you want

// import from private key
auto decryptor = make_shared<PaillierEncryption>(pk, sk1, sk2, GP_Q, GP_P, GP_G);

// import form public key
auto encryptor = make_shared<PaillierEncryption>(pk, GP_Q, GP_P, GP_G);
```

### Message format

```c++
// binary and big integer conversion
auto m1Binary = vector<uint8_t>({0xcd, 0xab, 0x34, 0x12}); // 0x1234abcd
auto m1Num = ConvertUtils::toZZ(m1Binary);                 // 305441741

// structured message
// eg. message size is 8 bytes, and slot size is 4 bytes
auto m2Binary = vector<uint8_t>({1, 0, 0, 0, 1, 0, 0, 0});
```

### Example for ZKP prove and verify flows

Please refer to the benchmark test script ([./src/cpp/app/App.cpp](./src/cpp/app/App.cpp))

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
