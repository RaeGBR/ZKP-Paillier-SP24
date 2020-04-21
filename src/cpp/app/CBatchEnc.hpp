#pragma once

#include "./namespace.hpp"

#include "./CBase.hpp"
#include "./CEnc.hpp"
#include "./PaillierEncryption.hpp"
#include "./CircuitZKPVerifier.hpp"
#include "./utils/ConvertUtils.hpp"
#include "./math/MathUtils.hpp"

#include "./math/Matrix.hpp"

namespace polyu
{

/**
 * @brief _CBatchEnc_ represents a circuit for paillier encryption of multiple structured messages, it inherits from _CBase_. It help you to generate the linear constrains and assign values to circuit arguments base on the given inputs (ciphertexts, original messages or randomnesses). Internally it reuses the _CEnc_ object, and aggregates to a larger circuit with additional constrains.
 */
class CBatchEnc : public CBase
{
public:
  /// @brief  PaillierEncryption parameters, either public key or private-key-public-key pair
  shared_ptr<PaillierEncryption> crypto;

  /// @brief Message size = N's size = public key length (in byte)
  size_t msgSize;

  /// @brief Total messages count
  size_t msgCount;

  /// @brief Messages per batch
  size_t msgPerBatch;

  /// @brief Total batch count = (msgCount / msgPerBatch)
  size_t batchCount;

  /// @brief Message's slot size (in byte)
  size_t slotSize;

  /// @brief Slots per message = (msgSize / slotSize)
  size_t slotsPerMsg;

  /// @brief Total range proof count, ie. number of linear constrains: l * b + R_j = L_j
  size_t rangeProofCount;

  /// @brief Maximum bound of R_j = 2 ^ 282 ~ 2 ^ * (slotsPerMsg * msgPerBatch / 3.4)
  ZZ RjMax;

  /// @brief Original messages (mi)
  Vec<ZZ> m;

  /// @brief Auxiliary messages (m*_s)
  Vec<ZZ> m_;

  /// @brief Range proof masks (R'_j)
  Vec<ZZ_p> Rj;

  /// @brief randomness of message
  Vec<ZZ_p> Rm;

  /// @brief Randomness of auxiliary messages
  Vec<ZZ_p> Rm_;

  /// @brief Randomness of range proof masks
  Vec<ZZ_p> RRj;

  /// @brief Ciphertexts of messages
  Vec<ZZ_p> Cm;

  /// @brief Ciphertexts of auxiliary messages
  Vec<ZZ_p> Cm_;

  /// @brief Ciphertexts of range proof masks
  Vec<ZZ_p> CRj;

  using CBase::CBase;

  /**
   * @brief Construct a new CBatchEnc object
   *
   * @param crypto PaillierEncryption parameters, either public key or private-key-public-key pair
   * @param msgCount Total messages count
   * @param rangeProofCount Total range proof count, ie. number of linear constrains: l * b + R_j = L_j
   * @param slotSize Message's slot size (in byte)
   * @param msgPerBatch Messages per batch
   */
  CBatchEnc(const shared_ptr<PaillierEncryption> &crypto,
            size_t msgCount, size_t rangeProofCount = 2,
            size_t slotSize = 4, size_t msgPerBatch = 15);

  /**
   * @brief Estimate the total number of multiplication constrains base on the public key (N)
   *
   * @return size_t
   */
  size_t estimateGateCount();

  /**
   * @brief Estimate the number of generators required to run the ZKP base on the circuit settings
   *
   * @return size_t
   */
  size_t estimateGeneratorsRequired();

  /**
   * @brief Encrypt a batch of messages
   *
   * @param msg Original messages
   */
  void encrypt(const Vec<ZZ> &msg);

  /**
   * @brief Set ciphertext to the circuit, update related constrains
   *
   * @param Cm Ciphertext of messages
   * @param Cm_ Ciphertext of auxiliary messages
   * @param CRj Ciphertext of range proof masks
   */
  void setCipher(const Vec<ZZ_p> &Cm,
                 const Vec<ZZ_p> &Cm_,
                 const Vec<ZZ_p> &CRj);

  /**
   * @brief Get the length of challenge value L_j,i,r
   *
   * @return size_t
   */
  size_t getLjLength();

  /**
   * @brief Calculate the challenge value (L_j,i,r) base on the ciphertext, used for non-interactive mode
   *
   * @return binary_t
   */
  binary_t calculateLjir();

  /**
   * @brief Calculate challenge response (L_j = l * b + R_j) for range proof
   *
   * @param Ljir Challenge value L_j,i,r
   * @return Vec<ZZ_p>
   */
  Vec<ZZ_p> calculateLj(const binary_t &Ljir);

  /**
   * @brief Wire up the circuit, build the linear constrains (w_q,a, w_q,b, w_q,c, K_q)
   *
   * @param Ljir Challenge value L_j,i,r
   * @param Lj Challenge response (L_j = l * b + R_j) for range proof
   */
  void wireUp(const binary_t &Ljir, const Vec<ZZ_p> &Lj);

  /**
   * @brief Run the circuit, assign values to the circuit's arguments (A, B, C)
   *
   * @param Ljir Challenge value L_j,i,r
   * @param Lj Challenge response (L_j = l * b + R_j) for range proof
   */
  void run(const binary_t &Ljir, const Vec<ZZ_p> &Lj);
};

} // namespace polyu
