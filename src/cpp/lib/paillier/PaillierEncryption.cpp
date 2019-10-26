#include "PaillierEncryption.hpp"

using namespace std;
using namespace cryptoplus;

// Generate the Paillier Cryptosystem
shared_ptr<PaillierEncryption> PaillierEncryption::generate(int byteLength, const vector<uint8_t> &seed) {
  if (seed.size() == 0) {
    return make_shared<PaillierEncryption>(byteLength);
  } else {
    return make_shared<PaillierEncryption>(byteLength, seed);
  }
}

// Generate a new Keypair
PaillierEncryption::PaillierEncryption(int byteLength) {
  this->byteLength = byteLength;
  auto length = byteLength >> 1;
  // P & Q are big primes which byte length roughly ~= N's byte length / 2
  auto p = Random::genInteger(length, true);
  auto q = Random::genInteger(length, true);
  this->n = p->mul(q);

  // Loop until GCD (pq, (p-1)(q-1)) == 1 and byte length of (pq) = N
  while (this->n->toBinary().size() != byteLength || p->compare(q) == 0) {
    p = Random::genInteger(length, true);
    q = Random::genInteger(length, true);

    this->n = p->mul(q);
  }

  this->lambda = p->sub(Integer::ONE())->mul(q->sub(Integer::ONE()));

  this->n2 = this->n->mul(this->n);

  this->g = this->n->add(Integer::ONE())->mod(this->n2);  // Burden code ? N+1 < N^2 as always...
  this->mu = this->lambda->inv(this->n);
}

// Generate a new Keypair with seed
PaillierEncryption::PaillierEncryption(int byteLength, vector<uint8_t> seed) {
  this->byteLength = byteLength;
  auto length = byteLength >> 1;

  auto iseed = Integer::createWithBinary(seed);

  // P & Q are big primes which byte length roughly ~= N's byte length / 2
  auto p = Random::genInteger(length, true, seed);
  auto q = Random::genInteger(length, true, iseed->add(Integer::ONE())->toBinary());
  this->n = p->mul(q);

  while (this->n->toBinary().size() != byteLength || p->compare(q) == 0) {
    p = Random::genInteger(length, true, iseed->add(Integer::ONE())->toBinary());
    q = Random::genInteger(length, true, iseed->add(Integer::ONE())->toBinary());

    this->n = p->mul(q);
  };

  this->lambda = p->sub(Integer::ONE())->mul(q->sub(Integer::ONE()));

  this->n2 = this->n->mul(this->n);

  this->g = this->n->add(Integer::ONE())->mod(this->n2);  // Burden code ? N+1 < N^2 as always...
  this->mu = this->lambda->inv(this->n);
}

// Import KeyPair From Lambda
PaillierEncryption::PaillierEncryption(const std::shared_ptr<Integer> &N, const std::shared_ptr<Integer> &lamda) {
  this->n = N;
  this->byteLength = this->n->toBinary().size();
  this->lambda = lamda;

  this->n2 = this->n->mul(this->n);
  this->g = this->n->add(Integer::ONE())->mod(this->n2);
  this->mu = this->lambda->inv(this->n);
}

std::shared_ptr<Integer> PaillierEncryption::getPrivateKey()
{
  return lambda;
}
shared_ptr<Integer> PaillierEncryption::getPublicKey()
{
  return n;
}

}
