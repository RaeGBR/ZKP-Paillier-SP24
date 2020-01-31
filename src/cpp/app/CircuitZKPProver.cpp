#include "./CircuitZKPProver.hpp"

using namespace polyu;

CircuitZKPProver::CircuitZKPProver(
    const shared_ptr<CircuitZKPVerifier> &zkp,
    const shared_ptr<Matrix> &A,
    const shared_ptr<Matrix> &B,
    const shared_ptr<Matrix> &C)
{
  this->zkp = zkp;
  this->A = A;
  this->B = B;
  this->C = C;

  if (A->m != zkp->m || A->n != zkp->n || B->m != zkp->m || B->n != zkp->n || C->m != zkp->m || C->n != zkp->n)
    throw invalid_argument("the matrix dimension of A, B and C are different to linear constrains");
}

vector<shared_ptr<Integer>> CircuitZKPProver::commit()
{
  vector<shared_ptr<Integer>> ret;
  auto polyCom = make_shared<PolynomialCommitment>(zkp->GP_Q, zkp->GP_P, zkp->GP_G, zkp->n);

  randA.clear();
  randB.clear();
  randC.clear();
  randA = Random::getRandoms(zkp->m, zkp->GP_P);
  randB = Random::getRandoms(zkp->m, zkp->GP_P);
  randC = Random::getRandoms(zkp->m, zkp->GP_P);
  D = Random::getRandoms(zkp->n, zkp->GP_P);
  randD = Random::genInteger(zkp->GP_P);

  for (size_t i = 0; i < zkp->m; i++)
    ret.push_back(polyCom->commit(A->values[i], randA[i]));
  for (size_t i = 0; i < zkp->m; i++)
    ret.push_back(polyCom->commit(B->values[i], randB[i]));
  for (size_t i = 0; i < zkp->m; i++)
    ret.push_back(polyCom->commit(C->values[i], randC[i]));

  ret.push_back(polyCom->commit(D, randD));

  return ret;
}

vector<shared_ptr<Integer>> CircuitZKPProver::polyCommit(const shared_ptr<Integer> &y)
{
  const size_t m = zkp->m;
  const auto p = zkp->GP_P;

  zkp->setY(y);                     // recalculate cachedY_ and cachedY_Mq
  auto Y = zkp->getY(y)->values[0]; // [1, y, y^2, ... , y^m]
  auto Y_ = zkp->getY_(y);          // [y^m, y^2m, ... , y^mn]

  // r(X) = SUM(ai * y^i * X^i) + SUM(bi * X^-i) + X^m * SUM(ci * X^i) + d * X^2m+1
  auto rx = make_shared<Polynomial>();
  for (size_t i = 1; i <= m; i++)
  {
    auto ai = make_shared<Matrix>(A->values[i - 1]);
    ai = ai->mul(Y[i], p);
    rx->put(i, ai);

    auto bi = make_shared<Matrix>(B->values[i - 1]);
    rx->put(-i, bi);

    auto ci = make_shared<Matrix>(C->values[i - 1]);
    rx->put(m + i, ci);
  }
  auto di = make_shared<Matrix>(D);
  rx->put(2 * m + 1, di);

  // s(X) = SUM(Wai(y) * y^-i * X^-i) + SUM(Wbi(y) * X^i) + X^-m * SUM(Wci(y) * X^-i)
  auto sx = zkp->createSx(y);

  // r_(X) = r(X) inner Y_ + 2 * s(X)
  auto rx_ = rx->inner(Y_, p);
  auto sx2 = sx->mul(Integer::TWO(), p);
  rx_ = rx_->add(sx2, p);

  // t(X) = r(X) * r_(X) - 2K(y)
  auto ky = p->sub(zkp->K(y)->modMul(Integer::TWO(), p));
  auto kyMatrix = make_shared<Matrix>(vector<shared_ptr<Integer>>({ky}));
  auto kyPoly = make_shared<Polynomial>();
  kyPoly->put(0, kyMatrix);
  auto tx = rx->mul(rx_->t(), p);
  tx = tx->add(kyPoly, p);

  if (!tx->get(0)->eq(Matrix::ZERO()))
    throw invalid_argument("t0 should be zero, the arguments A, B, C do not match with constrains Wa, Wb, Wc, Kq");

  int d1 = zkp->txM1 * zkp->txN;
  int d2 = zkp->txM2 * zkp->txN;
  vector<shared_ptr<Integer>> ti;
  for (int i = d1; i >= 1; i--)
  {
    ti.push_back(tx->get(-i)->values[0][0]);
  }
  for (int i = 1; i <= d2; i++)
  {
    ti.push_back(tx->get(i)->values[0][0]);
  }
  txT = zkp->commitScheme->calcT(zkp->txM1, zkp->txM2, zkp->txN, ti);
  txRi.clear();
  txRi = Random::getRandoms(zkp->txM1 + zkp->txM2 + 1, p);

  return zkp->commitScheme->commit(zkp->txM1, zkp->txM2, zkp->txN, txT, txRi);
}

vector<shared_ptr<Integer>> CircuitZKPProver::prove(const shared_ptr<Integer> &y, const shared_ptr<Integer> &x)
{
  // proofs contains: (pe..., r..., rr)
  vector<shared_ptr<Integer>> ret = zkp->commitScheme->eval(zkp->txM1, zkp->txM2, zkp->txN, txT, txRi, x);

  const auto m = zkp->m;
  const auto n = zkp->n;
  const auto p = zkp->GP_P;

  // r =   SUM(ai * x^i * y^i) +  SUM(bi * x^-i) + x^m *  SUM(ci * x^i) + d * x^(2m+1)
  // rr = SUM(rai * x^i * y^i) + SUM(rbi * x^-i) + x^m * SUM(rci * x^i) + d * x^(2m+1)
  auto prevX = Integer::ONE();
  auto prevY = Integer::ONE();
  auto prevXM = x->modPow(make_shared<IntegerImpl>(m), p);
  auto r = make_shared<Matrix>(1, n);
  auto rr = Integer::ZERO();
  for (int i = 1; i <= m; i++)
  {
    auto xi = prevX = prevX->modMul(x, p);
    auto yi = prevY = prevY->modMul(y, p);
    auto xmi = prevXM = prevXM->modMul(x, p);
    auto xi_ = xi->inv(p);

    auto a = make_shared<Matrix>(A->values[i - 1]);
    auto b = make_shared<Matrix>(B->values[i - 1]);
    auto c = make_shared<Matrix>(C->values[i - 1]);
    a = a->mul(xi, p)->mul(yi, p);
    b = b->mul(xi_, p);
    c = c->mul(xmi, p);
    r = r->add(a, p)->add(b, p)->add(c, p);

    auto ra = randA[i - 1];
    auto rb = randB[i - 1];
    auto rc = randC[i - 1];
    ra = ra->modMul(xi, p)->modMul(yi, p);
    rb = rb->modMul(xi_, p);
    rc = rc->modMul(xmi, p);
    rr = rr->add(ra)->add(rb)->add(rc)->mod(p);
  }
  auto x2m1 = x->modPow(make_shared<IntegerImpl>(2 * m + 1), p);
  auto d = make_shared<Matrix>(D);
  d = d->mul(x2m1, p);
  r = r->add(d, p);

  auto rd = randD;
  rd = rd->modMul(x2m1, p);
  rr = rr->add(rd)->mod(p);

  ret.insert(ret.end(), r->values[0].begin(), r->values[0].end());
  ret.push_back(rr);

  return ret;
}
