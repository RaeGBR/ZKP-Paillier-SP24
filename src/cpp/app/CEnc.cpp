#include "./CEnc.hpp"

using namespace polyu;

CEnc::CEnc(const shared_ptr<PaillierEncryption> &crypto)
    : CBase::CBase(crypto->getGroupQ(), crypto->getGroupP(), crypto->getGroupG())
{
  N = crypto->getPublicKey();
}

void CEnc::wireUp(const ZZ_p &C)
{
  ZZ_pPush push(GP_P);
  const auto NEG_ONE = conv<ZZ_p>(-1);
  const auto ONE = conv<ZZ_p>(1);

  // gate: m * N = mN
  auto n = addGate();

  // linear: b0 = N
  auto q = addLinear();

  Wqb[q - 1]->cell(0, n - 1, ONE);
  Kq[q - 1] = conv<ZZ_p>(N);

  // eg. N = 101 = 0b 110 0101
  //       = 2^6 + 2^5 + 2^2 + 2^0
  auto nBinStr = ConvertUtils::toBinaryString(N);

  auto maxPow = nBinStr.size() - 1;

  if (maxPow < 2)
    throw invalid_argument("N is too small");

  // gate: r * r = r^2
  n = addGate();
  // linear: a1 - b1 = 0
  q = addLinear();
  Wqa[q - 1]->cell(0, n - 1, ONE);
  Wqb[q - 1]->cell(0, n - 1, NEG_ONE);

  // gate: r^x * r^x = r^y
  // eg. r^2 r^2 = r^4
  for (size_t i = 2; i <= maxPow; i++)
  {
    n = addGate();

    // linear: a2 - c1 = 0
    q = addLinear();
    Wqa[q - 1]->cell(0, n - 1, ONE);
    Wqc[q - 1]->cell(0, n - 2, NEG_ONE);

    // linear: b2 - c1 = 0
    q = addLinear();
    Wqb[q - 1]->cell(0, n - 1, ONE);
    Wqc[q - 1]->cell(0, n - 2, NEG_ONE);
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
    Wqa[q - 1]->cell(0, n - 1, ONE);
    if (aggregateCnt == 2 && firstPow2 == 0)
      Wqa[q - 1]->cell(0, 1, NEG_ONE);
    else if (aggregateCnt == 2)
      Wqc[q - 1]->cell(0, firstPow2, NEG_ONE);
    else
      Wqc[q - 1]->cell(0, n - 2, NEG_ONE); // input a <- last output

    // linear: bi - cj = 0
    q = addLinear();
    Wqb[q - 1]->cell(0, n - 1, ONE);
    Wqc[q - 1]->cell(0, curPow2, NEG_ONE);
  }

  // gate: T * r^N = c
  n = addGate();

  // linear: T - mN = 1
  q = addLinear();
  Wqa[q - 1]->cell(0, n - 1, ONE);
  Wqc[q - 1]->cell(0, 0, NEG_ONE);
  Kq[q - 1] = ONE;

  // linear: bi - r^N = 0
  q = addLinear();
  Wqb[q - 1]->cell(0, n - 1, ONE);
  Wqc[q - 1]->cell(0, n - 2, NEG_ONE);

  // linear: ci = C
  q = addLinear();
  Wqc[q - 1]->cell(0, n - 1, ONE);
  Kq[q - 1] = C;
}

void CEnc::updateCipher(const ZZ_p &C)
{
  Kq[linearCount - 1] = C;
}

/*
 * A[0][0] = m
 * A[0][1] = r
 * C[0][q-1] = c
 * Kq[0][q-1] = c
 */
void CEnc::run(const ZZ &m, const ZZ_p &r)
{
  ZZ_pPush push(GP_P);
  ZZ_p tmp;

  auto n = 0;

  // gate: m * N = mN
  A->cell(0, n, conv<ZZ_p>(m));
  B->cell(0, n, conv<ZZ_p>(N));
  mul(tmp, A->cell(0, n), B->cell(0, n));
  C->cell(0, n, tmp);
  n++;

  // eg. N = 101 = 0b 110 0101
  //       = 2^6 + 2^5 + 2^2 + 2^0
  auto nBinStr = ConvertUtils::toBinaryString(N);
  auto maxPow = nBinStr.size() - 1;

  if (maxPow < 2)
    throw invalid_argument("N is too small");

  // gate: r * r = r^2
  A->cell(0, n, r);
  B->cell(0, n, r);
  mul(tmp, A->cell(0, n), B->cell(0, n));
  C->cell(0, n, tmp);
  n++;

  // gate: r^x * r^x = r^y
  // eg. r^2 r^2 = r^4
  for (size_t i = 2; i <= maxPow; i++)
  {
    A->cell(0, n, C->cell(0, n - 1));
    B->cell(0, n, C->cell(0, n - 1));
    mul(tmp, A->cell(0, n), B->cell(0, n));
    C->cell(0, n, tmp);
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
      A->cell(0, n, A->cell(0, 1));
    else if (aggregateCnt == 2)
      A->cell(0, n, C->cell(0, firstPow2));
    else
      A->cell(0, n, C->cell(0, n - 1)); // input a <- last output

    B->cell(0, n, C->cell(0, curPow2));
    mul(tmp, A->cell(0, n), B->cell(0, n));
    C->cell(0, n, tmp);
    n++;
  }

  // gate: T * r^N = c
  add(tmp, C->cell(0, 0), 1);
  A->cell(0, n, tmp);
  B->cell(0, n, C->cell(0, n - 1));
  mul(tmp, A->cell(0, n), B->cell(0, n));
  C->cell(0, n, tmp);
  n++;
}
