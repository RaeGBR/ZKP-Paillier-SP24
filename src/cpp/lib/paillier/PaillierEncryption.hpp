#pragma once

#include "../namespace.hpp"
#include "Integer.hpp"
#include "../math/Random.hpp"

using namespace std;
using namespace cryptoplus;

namespace cryptoplus
{
class PaillierEncryption {

private:
  int32_t byteLength;
  // Keypair
  shared_ptr<Integer> lambda; // lambda = (p-1)(q-1) mod n
  shared_ptr<Integer> mu;     // mu = lambda^-1 mod n

  shared_ptr<Integer> n;  // n = p*q
  shared_ptr<Integer> g;  // g = (n+1) mod n^2

  // Cache for faster computation
  shared_ptr<Integer> n2; // n^2
public:
  // Generate a Cryptosystem
  static shared_ptr<PaillierEncryption> generate(int byteLength, const vector<uint8_t> &seed = vector<uint8_t>());

  // Generate a new Keypair
  PaillierEncryption(int byteLength);

  // Generate a new Keypair with Seed
  PaillierEncryption(int byteLength, vector<uint8_t> seed);

  // Import KeyPair From Lambda and N
  PaillierEncryption(const std::shared_ptr<Integer> &N, const std::shared_ptr<Integer> &lamda);

  // Export Private Key
  std::shared_ptr<Integer> getPrivateKey();
  // Export Public Key
  std::shared_ptr<Integer> getPublicKey();

  // Interface
  std::shared_ptr<Integer> encrypt(const std::shared_ptr<Integer> &m);
  std::shared_ptr<Integer> decrypt(const std::shared_ptr<Integer> &c);
  std::shared_ptr<Integer> add(const std::shared_ptr<Integer> &c1, const std::shared_ptr<Integer> &c2);
  std::shared_ptr<Integer> mul(const std::shared_ptr<Integer> &c1, const std::shared_ptr<Integer> &m2);
};

}
