#pragma once

#include "../namespace.hpp"
#include "Integer.hpp"
#include "../math/Random.hpp"

using namespace std;
using namespace cryptoplus;

namespace cryptoplus
{
class PaillierEncryption
{

private:
  int32_t byteLength;
  // Keypair
  shared_ptr<Integer> lambda; // lambda = (p-1)(q-1) mod n
  shared_ptr<Integer> mu;     // mu = lambda^-1 mod n

  shared_ptr<Integer> n; // n = p*q
  shared_ptr<Integer> g; // g = (n+1) mod n^2

  void init();                                                            // init value n2, g, mu
  void init2(const shared_ptr<Integer> &p, const shared_ptr<Integer> &q); // init value Q, G

public:
  // Cyclic group parameters
  shared_ptr<Integer> Q;  // Q
  shared_ptr<Integer> n2; // p = n^2
  shared_ptr<Integer> G;  // generator

  // Generate a Cryptosystem
  static shared_ptr<PaillierEncryption> generate(int byteLength, const vector<uint8_t> &seed = vector<uint8_t>());

  // Generate a new Keypair
  PaillierEncryption(int byteLength);

  // Generate a new Keypair with Seed
  PaillierEncryption(int byteLength, vector<uint8_t> seed);

  // Import KeyPair From N (public key), can perform encrypt
  PaillierEncryption(const std::shared_ptr<Integer> &N);

  // Import KeyPair From Lambda and N (private & public key), can perform encrypt, decrypt
  PaillierEncryption(const std::shared_ptr<Integer> &N, const std::shared_ptr<Integer> &lamda);

  // Import KeyPair From p, q and N (private & public key), can perform encrypt, decrypt
  PaillierEncryption(const shared_ptr<Integer> &N, const shared_ptr<Integer> &p, const shared_ptr<Integer> &q);

  // Export Private Key
  std::shared_ptr<Integer> getPrivateKey();
  // Export Public Key
  std::shared_ptr<Integer> getPublicKey();

  // Interface
  std::shared_ptr<Integer> encrypt(const std::shared_ptr<Integer> &m);
  std::shared_ptr<Integer> decrypt(const std::shared_ptr<Integer> &c);

  // If ciphers = false -> c2 is data not cipher
  std::shared_ptr<Integer> add(const std::shared_ptr<Integer> &c1, const std::shared_ptr<Integer> &c2, bool ciphers);

  // E(c1)^k = c1 * k
  std::shared_ptr<Integer> mul(const std::shared_ptr<Integer> &c1, const std::shared_ptr<Integer> &k);
};

} // namespace cryptoplus
