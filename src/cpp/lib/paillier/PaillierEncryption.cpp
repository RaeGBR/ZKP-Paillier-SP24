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

std::shared_ptr<Integer> PaillierEncryption::encrypt(const std::shared_ptr<Integer> &m) {
  auto r = Random::genInteger(this->byteLength, false)->mod(this->n);
  auto rn = r->modPow(this->n, this->n2);

  // c = (nm + 1) * r^n mod n^2
  return this->n->mul(m)->add(Integer::ONE())->mul(rn)->mod(this->n2);
}

std::shared_ptr<Integer> PaillierEncryption::decrypt(const std::shared_ptr<Integer> &c) {
  return c->modPow(this->lambda, this->n2)->sub(Integer::ONE())->div(this->n)->mul(this->mu)->mod(this->n);
}

// If ciphers = false -> c2 is data not cipher
std::shared_ptr<Integer> PaillierEncryption::add(const std::shared_ptr<Integer> &c1, const std::shared_ptr<Integer> &c2, bool ciphers) {
  if (ciphers) {
    return c1->mul(c2)->mod(this->n2);
  } else {
    return c1->mul(g->modPow(c2, this->n2))->mod(this->n2);
  }
}

// E(c1)^k = c1 * k
std::shared_ptr<Integer> PaillierEncryption::mul(const std::shared_ptr<Integer> &c1, const std::shared_ptr<Integer> &k) {
  return c1->modPow(k, this->n2);
}
