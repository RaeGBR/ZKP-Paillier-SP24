# Coding examples

## Generate Key

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

## Message format

```c++
// binary and big integer conversion
auto m1Binary = vector<uint8_t>({0xcd, 0xab, 0x34, 0x12}); // 0x1234abcd
auto m1Num = ConvertUtils::toZZ(m1Binary);                 // 305441741

// structured message
// eg. message size is 8 bytes, and slot size is 4 bytes
auto m2Binary = vector<uint8_t>({1, 0, 0, 0, 1, 0, 0, 0});
```

## Example for ZKP

Please refer to the benchmark test script ([./src/cpp/app/App.cpp](./src/cpp/app/App.cpp))
