#include "PaillierEncryption.hpp"

using namespace std;
using namespace cryptoplus;

// Generate the Paillier Cryptosystem
shared_ptr<PaillierEncryption> PaillierEncryption::generate(int byteLength, const vector<uint8_t> &seed)
{
  if (seed.size() == 0)
  {
    return make_shared<PaillierEncryption>(byteLength);
  }
  else
  {
    return make_shared<PaillierEncryption>(byteLength, seed);
  }
}

// Generate a new Keypair
PaillierEncryption::PaillierEncryption(int byteLength)
{
  auto length = byteLength >> 1;
  // P & Q are big primes which byte length roughly ~= N's byte length / 2
  auto p = Random::genInteger(length, true);
  auto q = Random::genInteger(length, true);
  this->n = p->mul(q);

  // Loop until GCD (pq, (p-1)(q-1)) == 1 and byte length of (pq) = N
  while (this->n->toBinary().size() != byteLength || p->compare(q) == 0)
  {
    p = Random::genInteger(length, true);
    q = Random::genInteger(length, true);

    this->n = p->mul(q);
  }

  this->lambda = p->sub(Integer::ONE())->mul(q->sub(Integer::ONE()));

  init();
  init2(p, q);
}

// Generate a new Keypair with seed
PaillierEncryption::PaillierEncryption(int byteLength, vector<uint8_t> seed)
{
  auto length = byteLength >> 1;

  auto iseed = Integer::createWithBinary(seed);

  // P & Q are big primes which byte length roughly ~= N's byte length / 2
  auto p = Random::genInteger(length, true, seed);
  auto q = Random::genInteger(length, true, iseed->add(Integer::ONE())->toBinary());
  this->n = p->mul(q);

  while (this->n->toBinary().size() != byteLength || p->compare(q) == 0)
  {
    p = Random::genInteger(length, true, iseed->add(Integer::ONE())->toBinary());
    q = Random::genInteger(length, true, iseed->add(Integer::ONE())->toBinary());

    this->n = p->mul(q);
  };

  this->lambda = p->sub(Integer::ONE())->mul(q->sub(Integer::ONE()));

  init();
  init2(p, q);
}

// internal no group elements
PaillierEncryption::PaillierEncryption(const std::shared_ptr<Integer> &N,
                                       const std::shared_ptr<Integer> &lamda)
{
  this->n = N;
  this->lambda = lamda;

  init();
}

// Import KeyPair From N (public key) and group elements, can perform encrypt
PaillierEncryption::PaillierEncryption(const shared_ptr<Integer> &N,
                                       const shared_ptr<Integer> &GP_Q,
                                       const shared_ptr<Integer> &GP_P,
                                       const shared_ptr<Integer> &GP_G)
    : PaillierEncryption::PaillierEncryption(N, Integer::ONE(), GP_Q, GP_P, GP_G) {}

// Import KeyPair From Lambda and N (private & public key), can perform encrypt, decrypt
PaillierEncryption::PaillierEncryption(
    const std::shared_ptr<Integer> &N,
    const std::shared_ptr<Integer> &lamda,
    const shared_ptr<Integer> &GP_Q,
    const shared_ptr<Integer> &GP_P,
    const shared_ptr<Integer> &GP_G)
    : PaillierEncryption::PaillierEncryption(N, lamda)
{
  // TODO: check group elements relationship
  // if (!GP_P->eq(n2) || GP_Q->lte(GP_P) || !GP_Q->isPrime() || !GP_G->modPow(GP_P, GP_Q)->eq(Integer::ONE()))
  //   throw invalid_argument("Group element is not correct");

  this->Q = GP_Q;
  this->G = GP_G;
}

// Import KeyPair From p, q and N (private & public key), can perform encrypt, decrypt
PaillierEncryption::PaillierEncryption(const shared_ptr<Integer> &N,
                                       const shared_ptr<Integer> &p,
                                       const shared_ptr<Integer> &q)
    : PaillierEncryption::PaillierEncryption(N, p->sub(Integer::ONE())->mul(q->sub(Integer::ONE())))
{
  init2(p, q);
}

// init value n2, g, mu
void PaillierEncryption::init()
{
  this->n2 = this->n->mul(this->n);
  this->g = this->n->add(Integer::ONE())->mod(this->n2); // Burden code ? N+1 < N^2 as always...
  this->mu = this->lambda->inv(this->n);

  this->Q = this->n2;
  this->G = Integer::ONE();
}

// init value Q, G
void PaillierEncryption::init2(const shared_ptr<Integer> &p, const shared_ptr<Integer> &q)
{
  shared_ptr<Integer> f;
  for (int i = 2; true; i++)
  {
    f = make_shared<IntegerImpl>(i);
    Q = f->mul(n2)->add(Integer::ONE());
    if (Q->isPrime())
      break;
  }

  auto Qbin = Q->toBinary();
  while (true)
  {
    auto y = Random::genInteger(Qbin);
    G = y->modPow(f, Q);

    if (!G->modPow(n2, Q)->eq(Integer::ONE()))
      continue;

    if (G->modPow(p, Q)->eq(Integer::ONE()))
      continue;
    if (G->modPow(q, Q)->eq(Integer::ONE()))
      continue;
    if (G->modPow(p->mul(q), Q)->eq(Integer::ONE()))
      continue;

    if (G->modPow(p->mul(p), Q)->eq(Integer::ONE()))
      continue;
    if (G->modPow(q->mul(q), Q)->eq(Integer::ONE()))
      continue;

    if (G->modPow(p->mul(p)->mul(q), Q)->eq(Integer::ONE()))
      continue;
    if (G->modPow(q->mul(q)->mul(p), Q)->eq(Integer::ONE()))
      continue;

    break;
  }
}

std::shared_ptr<Integer> PaillierEncryption::getPrivateKey()
{
  return lambda;
}
shared_ptr<Integer> PaillierEncryption::getPublicKey()
{
  return n;
}

std::shared_ptr<Integer> PaillierEncryption::getGroupQ()
{
  return Q;
}
std::shared_ptr<Integer> PaillierEncryption::getGroupP()
{
  return n2;
}
std::shared_ptr<Integer> PaillierEncryption::getGroupG()
{
  return G;
}

std::shared_ptr<Integer> PaillierEncryption::pickRandom()
{
  // 0 < r < N, gcd(r, n) = 1
  while (true)
  {
    auto r = Random::genInteger(n, {}, true);
    auto gcd = r->gcd(n);
    if (gcd->eq(Integer::ONE()))
    {
      return r;
    }
  }
}

std::shared_ptr<Integer> PaillierEncryption::encrypt(const std::shared_ptr<Integer> &m, const std::shared_ptr<Integer> &_r)
{
  auto r = _r;
  if (r->eq(Integer::ZERO()))
  {
    r = pickRandom();
  }
  auto rn = r->modPow(this->n, this->n2);

  // c = (nm + 1) * r^n mod n^2
  return this->n->mul(m)->add(Integer::ONE())->mul(rn)->mod(this->n2);
}

std::shared_ptr<Integer> PaillierEncryption::decrypt(const std::shared_ptr<Integer> &c)
{
  return c->modPow(this->lambda, this->n2)->sub(Integer::ONE())->div(this->n)->mul(this->mu)->mod(this->n);
}

// If ciphers = false -> c2 is data not cipher
std::shared_ptr<Integer> PaillierEncryption::add(const std::shared_ptr<Integer> &c1, const std::shared_ptr<Integer> &c2, bool ciphers)
{
  if (ciphers)
  {
    return c1->mul(c2)->mod(this->n2);
  }
  else
  {
    return c1->mul(g->modPow(c2, this->n2))->mod(this->n2);
  }
}

// E(c1)^k = c1 * k
std::shared_ptr<Integer> PaillierEncryption::mul(const std::shared_ptr<Integer> &c1, const std::shared_ptr<Integer> &k)
{
  return c1->modPow(k, this->n2);
}
