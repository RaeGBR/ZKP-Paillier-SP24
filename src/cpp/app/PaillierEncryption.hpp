#pragma once

#include "./namespace.hpp"

#include "NTL/ZZ.h"
#include "NTL/ZZ_p.h"
#include "NTL/vector.h"

namespace polyu
{

/**
 * @brief _PaillierEncryption_ represents the paillier group elements with at least a public key or a public-key-private-key pair. Depends on the value it has, it is used in encryption, decryption, commitment and zero-knowledge(ZKP) prove protocols. Also, there are some static function for you to generate a new key pair and find the generator in the group.
 */
class PaillierEncryption
{

private:
  ZZ f; // random f such that Q = (f * n2) + 1 is prime

  void init(bool skipGenerate = false); // init value n2, g, mu

public:
  /*
   * Keypair
   */
  /// @brief Private element (p), a prime number
  ZZ p = ZZ();

  /// @brief Private element (q), a prime number
  ZZ q = ZZ();

  /// @brief Public key (N), N = p * q
  ZZ n = ZZ();

  /// @brief Private key (lambda), lambda = (p-1) * (q-1)
  ZZ lambda;

  /// @brief Internal value(mu), mu = lambda^-1 mod n
  ZZ_p mu;

  /// @brief Internal value(g), g = (n+1) mod n^2
  ZZ_p g;

  /*
   * Cyclic group parameters
   */
  /// @brief Group element p, p = N^2
  ZZ n2;

  /// @brief Group element Q
  ZZ Q;

  /// @brief Group generator
  ZZ_p G;

  /**
   * @brief Generate a new Keypair
   *
   * @param byteLength Key length
   */
  PaillierEncryption(size_t byteLength);

  /**
   * @brief Generate a new Keypair with Seed
   *
   * @param byteLength Key length
   * @param seed Seed
   */
  PaillierEncryption(size_t byteLength, const binary_t &seed);

  /**
   * @brief Import KeyPair from N (public key) and group elements, can perform encrypt
   *
   * @param N Public key (N)
   * @param GP_Q Group element Q
   * @param GP_P Group element p
   * @param GP_G Group generator
   */
  PaillierEncryption(const ZZ &N, const ZZ &GP_Q, ZZ &GP_P, ZZ_p &GP_G);

  /**
   * @brief Import KeyPair from Lambda and N (private & public key) and group elements, can perform encrypt, decrypt
   *
   * @param N Public key (N)
   * @param p Private element (p)
   * @param q Private element (q)
   * @param GP_Q Group element Q
   * @param GP_P Group element p
   * @param GP_G Group generator
   */
  PaillierEncryption(const ZZ &N, const ZZ &p, const ZZ &q, const ZZ &GP_Q, ZZ &GP_P, ZZ_p &GP_G);

  /**
   * @brief Import KeyPair from p, q and N (private & public key), can perform encrypt, decrypt
   *
   * @param N Public key (N)
   * @param p Private element (p)
   * @param q Private element (q)
   */
  PaillierEncryption(const ZZ &N, const ZZ &p, const ZZ &q);

  /**
   * @brief Generate a generator under the paillier group
   *
   * @return ZZ_p
   */
  ZZ_p genGenerator();

  /**
   * @brief Generate N generators under the paillier group
   *
   * @param n Number of generators needed
   * @return Vec<ZZ_p>
   */
  Vec<ZZ_p> genGenerators(size_t n);

  /**
   * @brief Get the private element (p)
   *
   * @return ZZ
   */
  ZZ getPrivateElement1();

  /**
   * @brief Get the private element (q)
   *
   * @return ZZ
   */
  ZZ getPrivateElement2();

  /**
   * @brief Get the public key
   *
   * @return ZZ
   */
  ZZ getPublicKey();

  /**
   * @brief Get the group element Q
   *
   * @return ZZ
   */
  ZZ getGroupQ();

  /**
   * @brief Get the group element p
   *
   * @return ZZ
   */
  ZZ getGroupP();

  /**
   * @brief Get the group generator g
   *
   * @return ZZ_p
   */
  ZZ_p getGroupG();

  /**
   * @brief Pick randomness (r) for encryption under public key (N), such that 0 < r < N, gcd(r, n) = 1
   *
   * @return ZZ_p
   */
  ZZ_p pickRandom();

  /**
   * @brief Encrypt a message
   *
   * @param m Original message
   * @return ZZ_p Ciphertext (c), the encrypted result
   */
  ZZ_p encrypt(const ZZ &m);

  /**
   * @brief Encrypt a message
   *
   * @param m Original message
   * @param r Randomness
   * @return ZZ_p Ciphertext (c), the encrypted result
   */
  ZZ_p encrypt(const ZZ &m, const ZZ_p &r);

  /**
   * @brief Decrypt a ciphertext
   *
   * @param c Ciphertext (c)
   * @return ZZ Original message (m)
   */
  ZZ decrypt(const ZZ_p &c);

  /**
   * @brief Homomorphic addition, m1 + m2 = Dec(c1 + c2)
   *
   * @param c1 Ciphertext 1
   * @param m2 Original message 2
   * @return ZZ_p c1 + c2
   */
  ZZ_p add(const ZZ_p &c1, const ZZ &m2);

  /**
   * @brief Homomorphic addition, m1 + m2 = Dec(c1 + c2)
   *
   * @param m1 Original message 1
   * @param c2 Ciphertext 2
   * @return ZZ_p c1 + c2
   */
  ZZ_p add(const ZZ &m1, const ZZ_p &c2);

  /**
   * @brief Homomorphic addition, m1 + m2 = Dec(c1 + c2)
   *
   * @param c1 Ciphertext 1
   * @param c2 Ciphertext 2
   * @return ZZ_p c1 + c2
   */
  ZZ_p add(const ZZ_p &c1, const ZZ_p &c2);

  // E(c)^k = c * k
  /**
   * @brief Homomorphic multiplication, m * k = Dec(c * k)
   *
   * @param c Ciphertext
   * @param k Multiplier
   * @return ZZ_p c * k
   */
  ZZ_p mul(const ZZ_p &c, const ZZ &k);
};

} // namespace polyu
