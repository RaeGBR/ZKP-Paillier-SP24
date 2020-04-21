#pragma once

#include "./namespace.hpp"

#include "./PaillierEncryption.hpp"
#include "./CBase.hpp"

#include "./math/Matrix.hpp"

namespace polyu
{

/**
 * @brief _CEnc_ represents a circuit for paillier encryption of a single message, it inherits from _CBase_. It help you to generate the linear constrains and assign values to circuit arguments base on the given inputs (ciphertext, original message or randomness).
 */
class CEnc : public CBase
{
public:
  /// @brief  Public key for paillier encryption
  ZZ N;

  using CBase::CBase;

  /**
   * @brief Construct a new CEnc object
   *
   * @param crypto
   */
  CEnc(const shared_ptr<PaillierEncryption> &crypto);

  /**
   * @brief Update the cipher text related constrains, used after circuit already wired up
   *
   * @param C Cipher text
   */
  void updateCipher(const ZZ_p &C);

  /**
   * @brief Wire up the circuit, build the linear constrains (w_q,a, w_q,b, w_q,c, K_q)
   *
   * @param C Cipher text
   */
  void wireUp(const ZZ_p &C = ZZ_p());

  /**
   * @brief Run the circuit, assign values to the circuit's arguments (A, B, C)
   *
   * @param m Original message
   * @param r Randomness
   */
  void run(const ZZ &m, const ZZ_p &r);
};

} // namespace polyu
