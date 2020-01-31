#include "./CircuitZKPVerifier.hpp"

using namespace polyu;

CircuitZKPVerifier::CircuitZKPVerifier(
    const shared_ptr<Integer> &GP_Q,
    const shared_ptr<Integer> &GP_P,
    const shared_ptr<Integer> &GP_G,
    const vector<shared_ptr<Matrix>> &Wqa,
    const vector<shared_ptr<Matrix>> &Wqb,
    const vector<shared_ptr<Matrix>> &Wqc,
    const vector<shared_ptr<Integer>> &Kq)
{
  this->GP_Q = GP_Q;
  this->GP_P = GP_P;
  this->GP_G = GP_G;
  this->Wqa = Wqa;
  this->Wqb = Wqb;
  this->Wqc = Wqc;
  this->Kq = Kq;

  this->Q = Wqa.size();
  if (this->Q <= 0 || this->Q != Wqb.size() || this->Q != Wqc.size())
    throw invalid_argument("the number of linear constrians are different in Wqa, Wqb and Wqc");
  this->m = Wqa[0]->m;
  this->n = Wqa[0]->n;
  this->N = m * n;
  this->M = N + m;

  auto txCfg = calcM1M2N(m);
  this->txM1 = txCfg[0];
  this->txM2 = txCfg[1];
  this->txN = txCfg[2];

  this->commitScheme = make_shared<PolynomialCommitment>(this->GP_Q, this->GP_P, this->GP_G, this->txN);
}

vector<size_t> CircuitZKPVerifier::calcMN(size_t _N)
{
  double N = _N;
  double n = ceil(sqrt(N));
  double m = ceil(N / n);
  vector<size_t> ret({(size_t)m, (size_t)n});
  return ret;
}

vector<size_t> CircuitZKPVerifier::calcM1M2N(size_t m)
{
  double N = 3 * m + 4 * m + 2;
  double n = ceil(sqrt(N));
  double m1 = ceil((3 * m) / n);
  double m2 = ceil((4 * m + 2) / n);
  vector<size_t> ret({(size_t)m1, (size_t)m2, (size_t)n});
  return ret;
}

vector<shared_ptr<Integer>> CircuitZKPVerifier::getY_Mq(const shared_ptr<Integer> &y)
{
  if (cachedY_Mq.size() > 0)
    return cachedY_Mq;

  vector<shared_ptr<Integer>> ret;
  auto M = make_shared<IntegerImpl>(this->M);
  auto y_M = y->modPow(M, GP_P);

  ret.push_back(y_M->modMul(y, GP_P)); // y^(M+1)
  for (size_t q = 1; q < Q; q++)
  {
    ret.push_back(ret[q - 1]->modMul(y, GP_P)); // y^(M+q)
  }
  cachedY_Mq = ret;
  return ret;
}

shared_ptr<Integer> CircuitZKPVerifier::getY_Mq(const shared_ptr<Integer> &y, size_t q)
{
  return getY_Mq(y)[q - 1];
}

shared_ptr<Matrix> CircuitZKPVerifier::getY(const shared_ptr<Integer> &y)
{
  if (cachedY != nullptr)
    return cachedY;

  auto Y = Matrix::powerVector(y, m + 1, GP_P); // [1, y, y^2, ... , y^m]
  cachedY = Y;
  return Y;
}

shared_ptr<Matrix> CircuitZKPVerifier::getY_(const shared_ptr<Integer> &y)
{
  if (cachedY_ != nullptr)
    return cachedY_;

  auto Y_ = make_shared<Matrix>(1, n);
  Y_->values[0][0] = y->modPow(make_shared<IntegerImpl>(m), GP_P); // y^m
  for (size_t i = 1; i < n; i++)
  {
    // y^m * previous object (so [y^m, y^2m, ... , y^mn])
    Y_->values[0][i] = Y_->values[0][i - 1]->modMul(Y_->values[0][0], GP_P);
  }
  cachedY_ = Y_;
  return Y_;
}

void CircuitZKPVerifier::setY(const shared_ptr<Integer> &y)
{
  this->cachedY_Mq.clear();
  this->cachedY = nullptr;
  this->cachedY_ = nullptr;
  getY_Mq(y);
  getY(y);
  getY_(y);
}

shared_ptr<Matrix> CircuitZKPVerifier::Wai(size_t i, const shared_ptr<Integer> &y)
{
  if (i <= 0 || i > m)
    throw invalid_argument("i should between 1 to m");

  auto ret = make_shared<Matrix>(1, n);
  for (size_t q = 1; q <= Q; q++)
  {
    auto w = make_shared<Matrix>(Wqa[q - 1]->values[i - 1]);
    w = w->mul(getY_Mq(y, q), GP_P);
    ret = ret->add(w, GP_P);
  }
  return ret;
}

shared_ptr<Matrix> CircuitZKPVerifier::Wbi(size_t i, const shared_ptr<Integer> &y)
{
  if (i <= 0 || i > m)
    throw invalid_argument("i should between 1 to m");

  auto ret = make_shared<Matrix>(1, n);
  for (size_t q = 1; q <= Q; q++)
  {
    auto w = make_shared<Matrix>(Wqb[q - 1]->values[i - 1]);
    w = w->mul(getY_Mq(y, q), GP_P);
    ret = ret->add(w, GP_P);
  }
  return ret;
}

shared_ptr<Matrix> CircuitZKPVerifier::Wci(size_t i, const shared_ptr<Integer> &y)
{
  if (i <= 0 || i > m)
    throw invalid_argument("i should between 1 to m");

  auto ret = make_shared<Matrix>(1, n);
  for (size_t q = 1; q <= Q; q++)
  {
    auto w = make_shared<Matrix>(Wqc[q - 1]->values[i - 1]);
    w = w->mul(getY_Mq(y, q), GP_P);
    ret = ret->add(w, GP_P);
  }

  auto Y_ = getY_(y);
  auto _y_i = Integer::ZERO()->sub(y->modPow(make_shared<IntegerImpl>(i), GP_P));
  auto Y_y_i = Y_->mul(_y_i, GP_P);

  ret = ret->add(Y_y_i, GP_P);
  return ret;
}

shared_ptr<Integer> CircuitZKPVerifier::K(const shared_ptr<Integer> &y)
{
  auto ret = Integer::ZERO();
  for (size_t q = 1; q <= Q; q++)
  {
    auto KqYmq = Kq[q - 1]->modMul(getY_Mq(y, q), GP_P);
    ret = ret->add(KqYmq)->mod(GP_P);
  }
  return ret;
}

shared_ptr<Polynomial> CircuitZKPVerifier::createSx(const shared_ptr<Integer> &y)
{
  // s(X) = SUM(Wai(y) * y^-i * X^-i) + SUM(Wbi(y) * X^i) + X^-m * SUM(Wci(y) * X^-i)
  auto sx = make_shared<Polynomial>();

  auto Y = getY(y)->values[0]; // [1, y, y^2, ... , y^m]

  for (size_t i = 1; i <= m; i++)
  {
    auto wai = Wai(i, y);
    auto yi = Y[i]->inv(GP_P);
    wai = wai->mul(yi, GP_P);
    sx->put(-i, wai);

    auto wbi = Wbi(i, y);
    sx->put(i, wbi);

    auto wci = Wci(i, y);
    sx->put(-m - i, wci);
  }
  return sx;
}

void CircuitZKPVerifier::setCommits(const vector<shared_ptr<Integer>> &commits)
{
  if (commits.size() != 3 * m + 1)
    throw invalid_argument("commitments count is not correct");

  commitA.clear();
  commitB.clear();
  commitC.clear();

  commitA.insert(commitA.begin(), commits.begin(), commits.begin() + m);
  commitB.insert(commitB.begin(), commits.begin() + m, commits.begin() + m + m);
  commitC.insert(commitC.begin(), commits.begin() + m + m, commits.begin() + m + m + m);
  commitD = commits[3 * m];
}

shared_ptr<Integer> CircuitZKPVerifier::calculateY()
{
  auto seed = commitD->toBinary();
  for (size_t i = 0; i < m; i++)
  {
    HexUtils::append(seed, commitA[i]->toBinary());
    HexUtils::append(seed, commitB[i]->toBinary());
    HexUtils::append(seed, commitC[i]->toBinary());
  }

  auto ret = Random::genInteger(GP_P, seed, true);
  return ret;
}

void CircuitZKPVerifier::setPolyCommits(const vector<shared_ptr<Integer>> &pc)
{
  if (pc.size() != txM1 + txM2 + 1)
    throw invalid_argument("polynomial commitments count is not correct");

  this->pc = pc;
}

shared_ptr<Integer> CircuitZKPVerifier::calculateX()
{
  auto seed = pc[0]->toBinary();
  for (size_t i = 1; i < pc.size(); i++)
  {
    HexUtils::append(seed, pc[i]->toBinary());
  }
  auto ret = Random::genInteger(GP_P, seed, true);
  return ret;
}

bool CircuitZKPVerifier::verify(const vector<shared_ptr<Integer>> &proofs, const shared_ptr<Integer> &y, const shared_ptr<Integer> &x)
{
  // check proof size
  if (proofs.size() != txN + 1 + n + 1)
    return false;

  vector<shared_ptr<Integer>> pe(proofs.begin(), proofs.begin() + txN + 1);
  vector<shared_ptr<Integer>> rArray(proofs.begin() + txN + 1, proofs.begin() + txN + 1 + n);
  auto r = make_shared<Matrix>(rArray);
  auto rr = proofs[proofs.size() - 1];

  // check PolyVerify
  if (!commitScheme->verify(txM1, txM2, txN, pc, pe, x))
    return false;

  // setting: u = 0
  // check PolyEval = r dot r' - 2K
  auto v1 = commitScheme->calcV(txN, pe, x);

  setY(y);                                                         // recalculate cachedY_ and cachedY_Mq
  auto s2 = createSx(y)->eval(x, GP_P)->mul(Integer::TWO(), GP_P); // 2s(x)
  auto r_ = r->inner(getY_(y), GP_P)->add(s2, GP_P);               // r' = r inner y' + 2s(x)
  auto rr_ = r->dot(r_, GP_P);                                     // r dot r'
  if (rr_->m != 1 || rr_->n != 1)
    return false;
  auto v2 = rr_->values[0][0]->sub(K(y)->modMul(Integer::TWO(), GP_P))->mod(GP_P); // r dot r' - 2K

  if (!v1->eq(v2))
    return false;

  // check commit(r, rr) is correct
  auto c1 = commitScheme->commit(rArray, rr);
  auto c2 = Integer::ONE();

  auto prevX = Integer::ONE();
  auto prevY = Integer::ONE();
  auto prevXM = x->modPow(make_shared<IntegerImpl>(m), GP_P);
  for (int i = 1; i <= m; i++)
  {
    auto xi = prevX = prevX->modMul(x, GP_P);
    auto yi = prevY = prevY->modMul(y, GP_P);
    auto xmi = prevXM = prevXM->modMul(x, GP_P);
    auto xi_ = xi->inv(GP_P);

    auto xiyi = xi->modMul(yi, GP_P);
    auto a = commitA[i - 1]->modPow(xiyi, GP_Q);
    auto b = commitB[i - 1]->modPow(xi_, GP_Q);
    auto c = commitC[i - 1]->modPow(xmi, GP_Q);
    c2 = c2->modMul(a, GP_Q)->modMul(b, GP_Q)->modMul(c, GP_Q);
  }
  auto x2m1 = x->modPow(make_shared<IntegerImpl>(2 * m + 1), GP_P);
  auto d = commitD->modPow(x2m1, GP_Q);
  c2 = c2->modMul(d, GP_Q);

  if (!c1->eq(c2))
    return false;

  return true;
}
