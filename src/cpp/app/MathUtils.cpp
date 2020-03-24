#include "./MathUtils.hpp"

ZZ_p MathUtils::randZZ_p(const ZZ &modulus, bool positiveOnly)
{
  ZZ_pPush push(modulus);
  ZZ_p ret;

  do
  {
    random(ret);
  } while (positiveOnly && IsZero(ret));

  return ret;
}

void MathUtils::randVecZZ_p(size_t n, const ZZ &modulus, Vec<ZZ_p> &ret, bool positiveOnly)
{
  ZZ_pPush push(modulus);
  ret.SetLength(n);

  for (size_t i = 0; i < n; i++)
  {
    do
    {
      random(ret[i]);
    } while (positiveOnly && IsZero(ret[i]));
  }
}

void MathUtils::powerVecZZ_p(const ZZ_p &xn, size_t n, const ZZ &modulus, Vec<ZZ_p> &ret)
{
  ret.SetLength(n);

  if (n == 0)
    return;

  ZZ_pPush push(modulus);

  ret[0] = 1;

  for (size_t i = 0; i < n - 1; i++)
  {
    mul(ret[i + 1], ret[i], xn);
  }
}
