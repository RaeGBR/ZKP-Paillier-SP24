#include "./PaillierEncryption.hpp"

// Generate a new Keypair
PaillierEncryption::PaillierEncryption(size_t byteLength)
    : PaillierEncryption(byteLength, binary_t()) {}

// Generate a new Keypair with seed
PaillierEncryption::PaillierEncryption(size_t byteLength, const binary_t &seed)
{
  if (seed.size() > 0)
  {
    SetSeed(seed.data(), seed.size());
  }

  // P & Q are big primes which byte length roughly ~= N's byte length / 2
  size_t length = byteLength << 2; // byteLength * 8 / 2

  do
  {
    GenPrime(p, length);
    GenPrime(q, length);
    n = p * q;
  } while (p == q);

  init();
  ZZ_p::init(Q);
}

// Import KeyPair from N (public key) and group elements, can perform encrypt
PaillierEncryption::PaillierEncryption(const ZZ &N, const ZZ &GP_Q, ZZ &GP_P, ZZ_p &GP_G)
{
  this->n = N;
  init();
  Q = GP_Q;
  n2 = GP_P;
  G = GP_G;
  ZZ_p::init(Q);
}

// Import KeyPair from Lambda and N (private & public key) and group elements, can perform encrypt, decrypt
PaillierEncryption::PaillierEncryption(const ZZ &N, const ZZ &p, const ZZ &q, const ZZ &GP_Q, ZZ &GP_P, ZZ_p &GP_G)
{
  this->n = N;
  this->p = p;
  this->q = q;
  init(true);
  Q = GP_Q;
  n2 = GP_P;
  G = GP_G;
  ZZ_p::init(Q);
}

// Import KeyPair From p, q and N (private & public key), can perform encrypt, decrypt
PaillierEncryption::PaillierEncryption(const ZZ &N,
                                       const ZZ &p,
                                       const ZZ &q)
{
  this->n = N;
  this->p = p;
  this->q = q;
  init();
  ZZ_p::init(Q);
}

// init value n2, g, mu
void PaillierEncryption::init(bool skipGenerate)
{
  ZZ_pPush push;

  // lambda = (p - 1) * (q - 1)
  if (p != 0 && q != 0)
  {
    lambda = (p - 1) * (q - 1);
  }

  // mu = inv(lambda) mod n
  if (lambda != 0)
  {
    ZZ_p::init(n);
    mu = inv(conv<ZZ_p>(lambda));
  }

  n2 = n * n; // p = n^2

  // g = (n+1) mod n^2
  ZZ_p::init(n2);
  g = conv<ZZ_p>(n + 1);

  // Cyclic group parameters
  // calculate value Q, G

  if (p == 0 || q == 0)
    return;

  for (size_t i = 2; true; i++)
  {
    f = i;
    Q = (f * n2) + 1;
    if (ProbPrime(Q))
      break;
  }

  if (skipGenerate)
    return;

  ZZ_p::init(Q);
  G = genGenerator();
}

ZZ_p PaillierEncryption::genGenerator()
{
  if (p == 0 || q == 0 || f == 0)
    throw invalid_argument("insufficient private into to generate a generator");

  ZZ_pPush push(Q);
  ZZ_p G0;
  ZZ_p y;
  while (true)
  {
    random(y);
    power(G0, y, f);

    ZZ_p x;
    power(x, G0, n2);
    if (x != 1)
      continue;

    power(x, G0, p);
    if (x == 1)
      continue;

    power(x, G0, q);
    if (x == 1)
      continue;

    power(x, G0, p * q);
    if (x == 1)
      continue;

    power(x, G0, p * p);
    if (x == 1)
      continue;

    power(x, G0, q * q);
    if (x == 1)
      continue;

    power(x, G0, p * p * q);
    if (x == 1)
      continue;

    power(x, G0, q * q * p);
    if (x == 1)
      continue;

    break;
  }
  return G0;
}

Vec<ZZ_p> PaillierEncryption::genGenerators(size_t n)
{
  Vec<ZZ_p> ret;
  ret.SetLength(n);
  for (size_t i = 0; i < n; i++)
  {
    ret[i] = genGenerator();
  }
  return ret;
}

ZZ PaillierEncryption::getPrivateElement1()
{
  return p;
}
ZZ PaillierEncryption::getPrivateElement2()
{
  return q;
}
ZZ PaillierEncryption::getPublicKey()
{
  return n;
}

ZZ PaillierEncryption::getGroupQ()
{
  return Q;
}
ZZ PaillierEncryption::getGroupP()
{
  return n2;
}
ZZ_p PaillierEncryption::getGroupG()
{
  return G;
}

ZZ_p PaillierEncryption::pickRandom()
{
  // 0 < r < N, gcd(r, n) = 1
  ZZ_pPush push(n);
  ZZ_p r;
  while (true)
  {
    random(r);
    if (r == 0)
      continue;

    auto gcd = GCD(conv<ZZ>(r), n);

    if (gcd == 1)
      return r;
  }
}
ZZ_p PaillierEncryption::encrypt(const ZZ &m)
{
  ZZ_pPush push(Q);
  return encrypt(m, ZZ_p());
}

ZZ_p PaillierEncryption::encrypt(const ZZ &m, const ZZ_p &_r)
{
  // c = g^m      * r^n mod n^2
  // c = (n+1)^m  * r^b mod n^2
  // c = (nm + 1) * r^n mod n^2
  ZZ_pPush push(n2);

  ZZ_p r = _r != 0 ? _r : pickRandom();
  ZZ_p c = conv<ZZ_p>(n * m + 1) * power(r, n);
  return c;
}

ZZ PaillierEncryption::decrypt(const ZZ_p &c)
{
  // L(x) = (x-1) / n
  // m = L(c^lambda mod n^2) * mu mod n
  ZZ_pPush push(n2);
  ZZ x = conv<ZZ>(power(c, lambda));

  ZZ l;
  div(l, (conv<ZZ>(x) - 1), n);

  ZZ_p::init(n);
  ZZ_p m = conv<ZZ_p>(l) * mu;
  return conv<ZZ>(m);
}

ZZ_p PaillierEncryption::add(const ZZ_p &c, const ZZ &m)
{
  auto c2 = encrypt(m);
  return add(c, c2);
}

ZZ_p PaillierEncryption::add(const ZZ &m, const ZZ_p &c)
{
  return add(c, m);
}

ZZ_p PaillierEncryption::add(const ZZ_p &c1, const ZZ_p &c2)
{
  ZZ_pPush push(n2);
  return c1 * c2;
}

ZZ_p PaillierEncryption::mul(const ZZ_p &c, const ZZ &k)
{
  ZZ_pPush push(n2);
  return power(c, k);
}
