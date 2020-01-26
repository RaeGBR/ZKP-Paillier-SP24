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

vector<shared_ptr<Integer>> CircuitZKPProver::getY_Mq(const shared_ptr<Integer> &y)
{
  if (cachedY_Mq.size() > 0)
    return cachedY_Mq;

  vector<shared_ptr<Integer>> ret;
  auto M = make_shared<IntegerImpl>(zkp->M);
  auto y_M = y->modPow(M, zkp->GP_P);

  ret.push_back(y_M->modMul(y_M, zkp->GP_P));
  for (size_t q = 1; q < zkp->Q; q++)
  {
    ret.push_back(ret[q - 1]->modMul(y_M, zkp->GP_P));
  }
  cachedY_Mq = ret;
  return ret;
}

shared_ptr<Integer> CircuitZKPProver::getY_Mq(const shared_ptr<Integer> &y, size_t q)
{
  return getY_Mq(y)[q - 1];
}

shared_ptr<Matrix> CircuitZKPProver::getY_(const shared_ptr<Integer> &y)
{
  if (cachedY_ != nullptr)
    return cachedY_;

  auto Y_ = make_shared<Matrix>(1, zkp->n);
  Y_->values[0][0] = y->modPow(make_shared<IntegerImpl>(zkp->m), zkp->GP_P); // y^m
  for (size_t i = 1; i < zkp->n; i++)
  {
    // y^m * previous object (so [y^m, y^2m, ... , y^mn])
    Y_->values[0][i] = Y_->values[0][i - 1]->modMul(Y_->values[0][0], zkp->GP_P);
  }
  cachedY_ = Y_;
  return Y_;
}

shared_ptr<Matrix> CircuitZKPProver::Wai(size_t i, const shared_ptr<Integer> &y)
{
  if (i <= 0 || i > zkp->m)
    throw invalid_argument("i should between 1 to m");

  size_t iIdx = i - 1;
  auto ret = make_shared<Matrix>(1, zkp->n);
  for (size_t q = 1; q <= zkp->Q; q++)
  {
    auto w = make_shared<Matrix>(zkp->Wqa[q - 1]->values[iIdx]);
    w = w->mul(getY_Mq(y, q), zkp->GP_P);
    ret = ret->add(w, zkp->GP_P);
  }
  return ret;
}

shared_ptr<Matrix> CircuitZKPProver::Wbi(size_t i, const shared_ptr<Integer> &y)
{
  if (i <= 0 || i > zkp->m)
    throw invalid_argument("i should between 1 to m");

  size_t iIdx = i - 1;
  auto ret = make_shared<Matrix>(1, zkp->n);
  for (size_t q = 1; q <= zkp->Q; q++)
  {
    auto w = make_shared<Matrix>(zkp->Wqb[q - 1]->values[iIdx]);
    w = w->mul(getY_Mq(y, q), zkp->GP_P);
    ret = ret->add(w, zkp->GP_P);
  }
  return ret;
}

shared_ptr<Matrix> CircuitZKPProver::Wci(size_t i, const shared_ptr<Integer> &y)
{
  if (i <= 0 || i > zkp->m)
    throw invalid_argument("i should between 1 to m");

  size_t iIdx = i - 1;
  auto ret = make_shared<Matrix>(1, zkp->n);
  for (size_t q = 1; q <= zkp->Q; q++)
  {
    auto w = make_shared<Matrix>(zkp->Wqa[q - 1]->values[iIdx]);
    w = w->mul(getY_Mq(y, q), zkp->GP_P);
    ret = ret->add(w, zkp->GP_P);
  }

  auto Y_ = getY_(y);
  auto _y_i = Integer::ZERO()->sub(y->modPow(make_shared<IntegerImpl>(i), zkp->GP_P));
  auto Y_y_i = Y_->mul(_y_i, zkp->GP_P);

  ret = ret->add(Y_y_i, zkp->GP_P);
  return ret;
}

shared_ptr<Integer> CircuitZKPProver::K(const shared_ptr<Integer> &y)
{
  auto ret = Integer::ZERO();
  for (size_t q = 1; q <= zkp->Q; q++)
  {
    auto KqYmq = zkp->Kq[q - 1]->modMul(getY_Mq(y, q), zkp->GP_P);
    ret = ret->add(KqYmq)->mod(zkp->GP_P);
  }
  return ret;
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
  size_t m = zkp->m;
  auto p = zkp->GP_P;

  auto rX = make_shared<Polynomial>();
  for (size_t i = 1; i <= m; i++)
  {
    auto iBigInt = make_shared<IntegerImpl>(i);
    size_t idx = i - 1;

    auto ai = make_shared<Matrix>(A->values[idx]);
    auto yi = y->modPow(iBigInt, p);
    ai = ai->mul(yi, p);
    rX->put(i, ai);

    auto bi = make_shared<Matrix>(B->values[idx]);
    rX->put(-i, bi);

    auto ci = make_shared<Matrix>(C->values[idx]);
    rX->put(m + i, ci);
  }
  auto di = make_shared<Matrix>(D);
  rX->put(2 * m + 1, di);

  // auto sX = make_shared<Polynomial>(-m * 2, m);

  return vector<shared_ptr<Integer>>();
}
