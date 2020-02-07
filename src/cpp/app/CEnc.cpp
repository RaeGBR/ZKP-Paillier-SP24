#include "./CEnc.hpp"

using namespace polyu;

CEnc::CEnc(const shared_ptr<PaillierEncryption> &crypto)
    : CBase::CBase(crypto->getGroupQ(), crypto->getGroupP(), crypto->getGroupG())
{
  N = crypto->getPublicKey();
}

void CEnc::wireUp(const shared_ptr<Integer> &C)
{
  const auto NEG_ONE = make_shared<IntegerImpl>(-1);
  const auto ONE = Integer::ONE();

  // gate: m * N = mN
  auto n = addGate();

  // linear: b0 = N
  auto q = addLinear();
  Wqb[q - 1]->values[0][n - 1] = ONE;
  Kq[q - 1] = N;

  // eg. N = 101 = 0b 110 0101
  //       = 2^6 + 2^5 + 2^2 + 2^0
  auto nBinStr = N->toBinaryString();
  auto maxPow = nBinStr.size() - 1;

  if (maxPow < 2)
    throw invalid_argument("N is too small");

  // gate: r * r = r^2
  n = addGate();
  // linear: a1 - b1 = 0
  q = addLinear();
  Wqa[q - 1]->values[0][n - 1] = ONE;
  Wqb[q - 1]->values[0][n - 1] = NEG_ONE;

  // gate: r^x * r^x = r^y
  // eg. r^2 r^2 = r^4
  for (size_t i = 2; i <= maxPow; i++)
  {
    n = addGate();

    // linear: a2 - c1 = 0
    q = addLinear();
    Wqa[q - 1]->values[0][n - 1] = ONE;
    Wqc[q - 1]->values[0][n - 2] = NEG_ONE;

    // linear: b2 - c1 = 0
    q = addLinear();
    Wqb[q - 1]->values[0][n - 1] = ONE;
    Wqc[q - 1]->values[0][n - 2] = NEG_ONE;
  }

  // gate: r^x * r^y = r^z ...
  // eg. r^1 * r^4 = r^5 ...
  size_t aggregateCnt = 0;
  size_t firstPow2 = 0;
  for (size_t i = 0; i < nBinStr.size(); i++)
  {
    size_t strIdx = nBinStr.size() - i - 1;
    if (nBinStr[strIdx] == '0')
      continue;

    if (aggregateCnt++ == 0)
    {
      firstPow2 = i;
      continue;
    }

    // 0=r^1=a1, 1=r^2=c1, 2=r^4=c2, ...
    size_t curPow2 = i;
    n = addGate();

    // linear: ai - cj = 0
    q = addLinear();
    Wqa[q - 1]->values[0][n - 1] = ONE;
    if (aggregateCnt == 2 && firstPow2 == 0)
      Wqa[q - 1]->values[0][1] = NEG_ONE;
    else if (aggregateCnt == 2)
      Wqc[q - 1]->values[0][firstPow2] = NEG_ONE;
    else
      Wqc[q - 1]->values[0][n - 2] = NEG_ONE; // input a <- last output

    // linear: bi - cj = 0
    q = addLinear();
    Wqb[q - 1]->values[0][n - 1] = ONE;
    Wqc[q - 1]->values[0][curPow2] = NEG_ONE;
  }

  // gate: T * r^N = c
  n = addGate();

  // linear: T - mN = 1
  q = addLinear();
  Wqa[q - 1]->values[0][n - 1] = ONE;
  Wqc[q - 1]->values[0][0] = NEG_ONE;
  Kq[q - 1] = ONE;

  // linear: bi - r^N = 0
  q = addLinear();
  Wqb[q - 1]->values[0][n - 1] = ONE;
  Wqc[q - 1]->values[0][n - 2] = NEG_ONE;

  // linear: ci = C
  q = addLinear();
  Wqc[q - 1]->values[0][n - 1] = ONE;
  Kq[q - 1] = C;
}

void CEnc::run(const shared_ptr<Integer> &m, const shared_ptr<Integer> &r)
{
  auto n = 0;

  // gate: m * N = mN
  A->values[0][n] = m;
  B->values[0][n] = N;
  C->values[0][n] = A->values[0][n]->modMul(B->values[0][n], GP_P);
  n++;

  // eg. N = 101 = 0b 110 0101
  //       = 2^6 + 2^5 + 2^2 + 2^0
  auto nBinStr = N->toBinaryString();
  auto maxPow = nBinStr.size() - 1;

  if (maxPow < 2)
    throw invalid_argument("N is too small");

  // gate: r * r = r^2
  A->values[0][n] = r;
  B->values[0][n] = r;
  C->values[0][n] = A->values[0][n]->modMul(B->values[0][n], GP_P);
  n++;

  // gate: r^x * r^x = r^y
  // eg. r^2 r^2 = r^4
  for (size_t i = 2; i <= maxPow; i++)
  {
    A->values[0][n] = C->values[0][n - 1];
    B->values[0][n] = C->values[0][n - 1];
    C->values[0][n] = A->values[0][n]->modMul(B->values[0][n], GP_P);
    n++;
  }

  // gate: r^x * r^y = r^z ...
  // eg. r^1 * r^4 = r^5 ...
  size_t aggregateCnt = 0;
  size_t firstPow2 = 0;
  for (size_t i = 0; i < nBinStr.size(); i++)
  {
    size_t strIdx = nBinStr.size() - i - 1;
    if (nBinStr[strIdx] == '0')
      continue;

    if (aggregateCnt++ == 0)
    {
      firstPow2 = i;
      continue;
    }

    // 0=r^1=a1, 1=r^2=c1, 2=r^4=c2, ...
    size_t curPow2 = i;
    if (aggregateCnt == 2 && firstPow2 == 0)
      A->values[0][n] = A->values[0][1];
    else if (aggregateCnt == 2)
      A->values[0][n] = C->values[0][firstPow2];
    else
      A->values[0][n] = C->values[0][n - 1]; // input a <- last output

    B->values[0][n] = C->values[0][curPow2];
    C->values[0][n] = A->values[0][n]->modMul(B->values[0][n], GP_P);
    n++;
  }

  // gate: T * r^N = c
  A->values[0][n] = C->values[0][0]->add(Integer::ONE())->mod(GP_P);
  B->values[0][n] = C->values[0][n - 1];
  C->values[0][n] = A->values[0][n]->modMul(B->values[0][n], GP_P);
  n++;
}
