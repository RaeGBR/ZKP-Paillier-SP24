#include "./PolynomialCommitment.hpp"

using namespace polyu;

PolynomialCommitment::PolynomialCommitment(
    const shared_ptr<Integer> &Q,
    const shared_ptr<Integer> &p,
    const shared_ptr<Integer> &g,
    size_t n)
{
  this->Q = Q;
  this->p = p;
  this->g = g;
  this->gi.push_back(g);

  auto gx = g;
  for (size_t i = 0; i < n; i++)
  {
    gx = gx->modMul(g, Q);
    this->gi.push_back(gx);
  }
}

shared_ptr<Integer> PolynomialCommitment::commit(
    const vector<shared_ptr<Integer>> &mi,
    const shared_ptr<Integer> &r)
{
  if (gi.size() < mi.size() + 1)
    throw invalid_argument("number of generators and messages do not match");

  auto ret = gi[0]->modPow(r, Q);

  for (size_t i = 0; i < mi.size(); i++)
  {
    auto gm = gi[i + 1]->modPow(mi[i], Q);
    ret = ret->modMul(gm, Q);
  }

  return ret;
}

shared_ptr<Matrix> PolynomialCommitment::calcT(
    size_t m1, size_t m2, size_t n,
    const vector<shared_ptr<Integer>> &tx)
{
  if ((m1 + m2) * n != tx.size())
    throw invalid_argument("m1, m2, n do not match with the t(x) length");
  if (m1 <= 0 || m2 <= 0)
    throw invalid_argument("m1, m2 must be positive integer");

  // group to m1+m2 x n matrix
  auto T = make_shared<Matrix>(tx);
  T = T->group(n);

  // Random vector u
  auto u = Random::getRandoms(n - 1, p);
  u.push_back(Integer::ZERO());
  T->appendRow(u);

  // mask vector t0'
  for (size_t i = 0; i < n - 1; i++)
  {
    (*T)[m1][i + 1] = (*T)[m1][i + 1]->sub(u[i])->mod(p);
  }
  return T;
}

vector<shared_ptr<Integer>> PolynomialCommitment::commit(
    size_t m1, size_t m2, size_t n,
    const shared_ptr<Matrix> &T,
    vector<shared_ptr<Integer>> &ri)
{
  size_t m = m1 + m2 + 1;
  if (T->m != m || T->n != n)
    throw invalid_argument("m1, m2, n do not match with the dimension of matrix T");
  if (ri.size() != 0 && ri.size() != m)
    throw invalid_argument("ri.size() do not match (m1 + m2 + 1)");

  // Generate randomness if it isn't provided
  if (ri.size() == 0)
  {
    auto rs = Random::getRandoms(m, p);
    ri.insert(ri.begin(), rs.begin(), rs.end());
  }

  // calculate commitment
  vector<shared_ptr<Integer>> ret;
  for (size_t i = 0; i < m; i++)
  {
    ret.push_back(commit((*T)[i], ri[i]));
  }

  return ret;
}

vector<shared_ptr<Integer>> PolynomialCommitment::eval(
    size_t m1, size_t m2, size_t n,
    const shared_ptr<Matrix> &T,
    const vector<shared_ptr<Integer>> &ri,
    const shared_ptr<Integer> &x)
{
  size_t m = m1 + m2 + 1;
  if (T->m != m || T->n != n)
    throw invalid_argument("m1, m2, n do not match with the dimension of matrix T");
  if (ri.size() != 0 && ri.size() != m)
    throw invalid_argument("ri.size() do not match (m1 + m2 + 1)");

  // Compute Z(X)
  vector<shared_ptr<Integer>> zx;
  for (size_t i = m1; i > 0; i--)
  {
    auto exp = make_shared<IntegerImpl>(i * n);
    auto z = x->modPow(exp, p);
    zx.push_back(z->inv(p));
  }
  zx.push_back(x);
  for (size_t i = 1; i < m2; i++)
  {
    auto exp = make_shared<IntegerImpl>((i * n) + 1);
    auto z = x->modPow(exp, p);
    zx.push_back(z);
  }
  zx.push_back(x->modPow(Integer::TWO(), p));
  auto Z = make_shared<Matrix>(zx);

  // t_ = Z * T
  auto t_ = Z->mul(T, p);

  // r_ = Z * r
  auto r_ = make_shared<Matrix>(ri);
  r_ = r_->t();

  r_ = Z->mul(r_, p);

  // return result vector
  vector<shared_ptr<Integer>> ret((*t_)[0].begin(), (*t_)[0].end());
  ret.push_back((*r_)[0][0]);
  return ret;
}

bool PolynomialCommitment::verify(
    size_t m1, size_t m2, size_t n,
    const vector<shared_ptr<Integer>> &pc,
    const vector<shared_ptr<Integer>> &pe,
    const shared_ptr<Integer> &x)
{
  size_t m = m1 + m2 + 1;
  if (pc.size() != m)
    throw invalid_argument("commitments count does not match with (m1 + m2 + 1)");
  if (pe.size() != n + 1)
    throw invalid_argument("evaluate results count does not match with (n + 1)");

  vector<shared_ptr<Integer>> t_(pe.begin(), pe.end() - 1);
  auto r = pe.back();
  auto c1 = commit(t_, r);

  // U ^ (x ^ 2)
  auto c2 = pc.back();
  {
    auto exp = x->modMul(x, p);
    c2 = c2->modPow(exp, Q);
  }

  // Product(Ti' ^ (x ^ ((i - m1) * n)))
  for (size_t i = 0; i < m1; i++)
  {
    auto exp = x->modPow(make_shared<IntegerImpl>((m1 - i) * n), p);
    exp = exp->inv(p);
    auto Ti = pc[i]->modPow(exp, Q);
    c2 = c2->modMul(Ti, Q);
  }

  // Product(Ti'' ^ (x ^ (i * n + 1)))
  for (size_t i = 0; i < m2; i++)
  {
    auto exp = x->modPow(make_shared<IntegerImpl>(i * n + 1), p);
    auto Ti = pc[m1 + i]->modPow(exp, Q);
    c2 = c2->modMul(Ti, Q);
  }

  // verify
  return c1->eq(c2);
}

shared_ptr<Integer> PolynomialCommitment::calcV(
    size_t n,
    const vector<shared_ptr<Integer>> &pe,
    const shared_ptr<Integer> &x)
{
  // v = t_ * X(x)
  vector<shared_ptr<Integer>> t_(pe.begin(), pe.end() - 1);
  auto t = make_shared<Matrix>(t_);
  auto X = Matrix::powerVector(x, n)->t();
  auto v = t->mul(X, p);
  return (*v)[0][0];
}
