#pragma once

#include "../lib/namespace.hpp"

#include "NTL/ZZ.h"
#include "NTL/ZZ_p.h"
#include "NTL/vector.h"

// #include "Integer.hpp"
// #include "../math/Random.hpp"

using namespace std;
using namespace NTL;

namespace polyu
{

class PaillierEncryption
{

private:
  // PaillierEncryption(const ZZ &N, const ZZ &lamda); // size_ternal no group elements
  void init(); // init value n2, g, mu
  // void init2(const ZZ &p, const ZZ &q);                     // init value Q, G

public:
  // Keypair
  ZZ p = ZZ(); // prime
  ZZ q = ZZ(); // prime
  ZZ n = ZZ(); // n = p*q
  ZZ lambda;   // lambda = (p-1)(q-1)
  ZZ_p mu;     // mu = lambda^-1 mod n
  ZZ_p g;      // g = (n+1) mod n^2

  // Cyclic group parameters
  ZZ n2;  // p = n^2
  ZZ Q;   // Q
  ZZ_p G; // generator

  // Generate a new Keypair
  PaillierEncryption(size_t byteLength);

  // Generate a new Keypair with Seed
  PaillierEncryption(size_t byteLength, const binary_t &seed);

  // Import KeyPair from N (public key) and group elements, can perform encrypt
  PaillierEncryption(const ZZ &N, const ZZ &GP_Q, ZZ &GP_P, ZZ_p &GP_G);

  // Import KeyPair from Lambda and N (private & public key) and group elements, can perform encrypt, decrypt
  PaillierEncryption(const ZZ &N, const ZZ &lambda, const ZZ &GP_Q, ZZ &GP_P, ZZ_p &GP_G);

  // Import KeyPair from p, q and N (private & public key), can perform encrypt, decrypt
  PaillierEncryption(const ZZ &N, const ZZ &p, const ZZ &q);

  // Export Private Key
  ZZ getPrivateKey();
  // Export Public Key
  ZZ getPublicKey();

  ZZ getGroupQ();
  ZZ getGroupP();
  ZZ_p getGroupG();

  // Interface
  ZZ_p pickRandom(); // 0 < r < N, gcd(r, n) = 1
  ZZ_p encrypt(const ZZ &m);
  ZZ_p encrypt(const ZZ &m, const ZZ_p &r);
  ZZ decrypt(const ZZ_p &c);

  // c + m = c + Enc(m)
  ZZ_p add(const ZZ_p &c, const ZZ &m);
  ZZ_p add(const ZZ &m, const ZZ_p &c);

  // Enc(m1 + m2) = c1 * c2
  ZZ_p add(const ZZ_p &c1, const ZZ_p &c2);

  // E(c)^k = c * k
  ZZ_p mul(const ZZ_p &c, const ZZ &k);
};

} // namespace polyu

using namespace polyu;
