#include "./CircuitZKPProver.hpp"

CircuitZKPProver::CircuitZKPProver(
    const shared_ptr<CircuitZKPVerifier> &zkp,
    const shared_ptr<Matrix> &A,
    const shared_ptr<Matrix> &B,
    const shared_ptr<Matrix> &C)
{
  ZZ_pPush push(zkp->GP_P);

  this->zkp = zkp;
  A->toMat(this->A);
  B->toMat(this->B);
  C->toMat(this->C);

  if (this->A.NumRows() != zkp->m || this->A.NumCols() != zkp->n ||
      this->B.NumRows() != zkp->m || this->B.NumCols() != zkp->n ||
      this->C.NumRows() != zkp->m || this->C.NumCols() != zkp->n)
    throw invalid_argument("the matrix dimension of A, B and C are different to linear constrains");
}

CircuitZKPProver::CircuitZKPProver(
    const shared_ptr<CircuitZKPVerifier> &zkp,
    const Mat<ZZ_p> &A,
    const Mat<ZZ_p> &B,
    const Mat<ZZ_p> &C)
{
  ZZ_pPush push(zkp->GP_P);

  this->zkp = zkp;
  this->A = A;
  this->B = B;
  this->C = C;

  if (this->A.NumRows() != zkp->m || this->A.NumCols() != zkp->n ||
      this->B.NumRows() != zkp->m || this->B.NumCols() != zkp->n ||
      this->C.NumRows() != zkp->m || this->C.NumCols() != zkp->n)
    throw invalid_argument("the matrix dimension of A, B and C are different to linear constrains");
}

void CircuitZKPProver::commit(Vec<ZZ_p> &ret)
{
  MathUtils::randVecZZ_p(zkp->m, zkp->GP_P, randA);
  MathUtils::randVecZZ_p(zkp->m, zkp->GP_P, randB);
  MathUtils::randVecZZ_p(zkp->m, zkp->GP_P, randC);
  MathUtils::randVecZZ_p(zkp->n, zkp->GP_P, D);
  randD = MathUtils::randZZ_p(zkp->GP_P);

  ret.SetLength(0);
  Vec<ZZ_p> tmp;
  zkp->commitScheme->commit(A, randA, tmp);
  ret.append(tmp);
  zkp->commitScheme->commit(B, randB, tmp);
  ret.append(tmp);
  zkp->commitScheme->commit(C, randC, tmp);
  ret.append(tmp);
  ret.append(zkp->commitScheme->commit(D, randD));
}

void CircuitZKPProver::polyCommit(const ZZ_p &y, Vec<ZZ_p> &ret)
{
  Timer::start("prover.polyCommit");
  const size_t m = zkp->m;
  const size_t n = zkp->n;

  Timer::start("prover.setY");
  zkp->setY(y);            // recalculate cachedY_ and cachedY_Mq
  auto Y = zkp->getY(y);   // [1, y, y^2, ... , y^m]
  auto Y_ = zkp->getY_(y); // [y^m, y^2m, ... , y^mn]
  Timer::end("prover.setY");

  ZZ_pPush push(zkp->GP_P);
  vector<ZZ_pX> rx;
  vector<ZZ_pX> sx;
  vector<ZZ_pX> rx_;
  ZZ_pX tx;
  ZZ_p tmp;
  ZZ_pX tmpX;

  for (size_t i = 0; i < n; i++)
  {
    rx.push_back(ZZ_pX());
    rx_.push_back(ZZ_pX());
  }

  // r(X) = SUM(ai * y^i * X^i) + SUM(bi * X^-i) + X^m * SUM(ci * X^i) + d * X^2m+1
  //      = (X^-m) * ( SUM(ai * y^i * X^(m+i)) + SUM(bi * X^(m-i)) + SUM(ci * X^(2m+i)) + d * X^3m+1 )
  Timer::start("prover.rx");
  const size_t m2 = m * 2;
  const size_t m3 = m * 3;
  for (size_t i = 1; i <= m; i++)
  {
    for (size_t j = 1; j <= n; j++)
    {
      mul(tmp, A(i, j), Y[i]);
      SetCoeff(rx[j - 1], m + i, tmp);

      SetCoeff(rx[j - 1], m - i, B(i, j));

      SetCoeff(rx[j - 1], m2 + i, C(i, j));
    }
  }
  for (size_t j = 1; j <= n; j++)
  {
    if (!IsZero(D(j)))
    {
      SetCoeff(rx[j - 1], m3 + 1, D(j));
    }
  }
  Timer::end("prover.rx");

  // s(X) = SUM(Wai(y) * y^-i * X^-i) + SUM(Wbi(y) * X^i) + X^-m * SUM(Wci(y) * X^-i)
  //      = (X^2m) * ( SUM(Wai(y) * y^-i * X^(2m-i)) + SUM(Wbi(y) * X^(2m+i)) + SUM(Wci(y) * X^(m-i)) )
  Timer::start("prover.sx");
  zkp->createSx(y, sx);
  Timer::end("prover.sx");

  // r_(X) = r(X) inner Y_ + 2 * s(X)
  //       = rX^-m + ... + rX^(2m+1) + sX^-2m + ... + sX^m
  //       = X^-m * (rX^0 + ... + rX^(3m+1)) + X^-2m * (sX^0 + ... + sX^3m)
  //       = X^-m * X^-m * X^m * (rX^0 + ... + rX^(3m+1)) + X^-2m * (sX^0 + ... + sX^3m)
  //       = X^-2m * (rX^m + ... + rX^(4m+1)) + X^-2m * (sX^0 + ... + sX^3m)
  Timer::start("prover.rx_");
  for (size_t j = 0; j < n; j++)
  {
    mul(tmpX, sx[j], 2);          // 2 * s(X)
    mul(rx_[j], rx[j], Y_[j]);    // r(X) inner Y_
    LeftShift(rx_[j], rx_[j], m); // * X^-m, degree shift m to the left
    add(rx_[j], rx_[j], tmpX);    // r(X) inner Y_ + 2 * s(X)
  }

  //   auto rx_ = rx->inner(Y_, p);
  //   auto sx2 = sx->mul(Integer::TWO(), p);
  //   rx_ = rx_->add(sx2, p);
  Timer::end("prover.rx_");

  // t(X) = r(X) * r_(X) - 2K(y)
  //      = X^(-3m) * ( r(X) * r_(X) - 2K(y) * X^3m )
  Timer::start("prover.ky");
  auto ky = zkp->K(y);
  ZZ_pX kyX;
  SetCoeff(kyX, m3, ky * 2);
  Timer::end("prover.ky");

  Timer::start("prover.tx.mul");
  // rx * rx_
  for (size_t i = 0; i < n; i++)
  {
    mul(tmpX, rx[i], rx_[i]);
    add(tx, tx, tmpX);
  }
  sub(tx, tx, kyX); // rx * rx_ - 2ky
  Timer::end("prover.tx.mul");

  if (!IsZero(tx[m3]))
    throw invalid_argument("t0 should be zero, the arguments A, B, C do not match with constrains Wa, Wb, Wc, Kq");

  // shift t(X) degree and calcT for polyCommit
  // t(X) = X^(-3m) * ( t0 + t1 * X^1 + ... )
  //      = X^(-txM1 * txN) * (...)
  Timer::start("prover.txT");
  size_t degDiff = zkp->txM1 * zkp->txN - m3;
  if (degDiff > 0)
  {
    LeftShift(tx, tx, degDiff);
  }
  zkp->commitScheme->calcT(zkp->txM1, zkp->txM2, zkp->txN, tx, txT);

  MathUtils::randVecZZ_p(zkp->txM1 + zkp->txM2 + 1, zkp->GP_P, txRi);
  Timer::end("prover.txT");

  Timer::start("prover.txCommit");
  // polyCommit( t(X) )
  zkp->commitScheme->commit(zkp->txM1, zkp->txM2, zkp->txN, txT, txRi, ret);
  Timer::end("prover.txCommit");

  Timer::end("prover.polyCommit");
}

void CircuitZKPProver::prove(const ZZ_p &y, const ZZ_p &x, Vec<ZZ_p> &ret)
{
  const auto m = zkp->m;
  const auto n = zkp->n;
  const auto p = zkp->GP_P;

  ZZ_pPush push(p);

  // proofs contains: (pe..., r..., rr)
  // Mat<ZZ_p> txT;
  // zkp->commitScheme->calcT(zkp->txM1, zkp->txM2, zkp->txN, tx, txT);
  // txTMat->toMat(txT);
  zkp->commitScheme->eval(zkp->txM1, zkp->txM2, zkp->txN, txT, txRi, x, ret);

  // r =   SUM(ai * x^i * y^i) +  SUM(bi * x^-i) + x^m *  SUM(ci * x^i) + d * x^(2m+1)
  // rr = SUM(rai * x^i * y^i) + SUM(rbi * x^-i) + x^m * SUM(rci * x^i) + d * x^(2m+1)
  ZZ_p xi = conv<ZZ_p>(1);
  ZZ_p yi = conv<ZZ_p>(1);
  ZZ_p xmi = power(x, m);
  ZZ_p xi_;

  Vec<ZZ_p> r;
  r.SetLength(n);
  ZZ_p rr;
  ZZ_p tmp;
  Vec<ZZ_p> tmpVec;

  for (int i = 1; i <= m; i++)
  {
    mul(xi, xi, x);   // x^1, x^2, ... x^m
    mul(yi, yi, y);   // y^1, y^2, ... y^m
    mul(xmi, xmi, x); // x^m+1, x^m+2, ..., x^2m
    inv(xi_, xi);     // x^-1, x^-2, ... x^-m

    // ai * x^i * y^i
    mul(tmpVec, A(i), xi);
    mul(tmpVec, tmpVec, yi);
    add(r, r, tmpVec);

    // bi * x^-i
    mul(tmpVec, B(i), xi_);
    add(r, r, tmpVec);

    // ci * x^m+i
    mul(tmpVec, C(i), xmi);
    add(r, r, tmpVec);

    // randAi * x^i * y^i
    mul(tmp, randA(i), xi);
    mul(tmp, tmp, yi);
    add(rr, rr, tmp);

    // randBi * x^-i
    mul(tmp, randB(i), xi_);
    add(rr, rr, tmp);

    // randCi * x^m+i
    mul(tmp, randC(i), xmi);
    add(rr, rr, tmp);
  }

  // D * x^2m+1
  mul(xmi, xmi, x); // x^2m+1
  mul(tmpVec, D, xmi);
  add(r, r, tmpVec);

  // randD * x^2m+1
  mul(tmp, randD, xmi);
  add(rr, rr, tmp);

  ret.append(r);
  ret.append(rr);
}
