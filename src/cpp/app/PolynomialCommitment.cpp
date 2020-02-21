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
  auto ms = make_shared<Matrix>(mi);
  vector<shared_ptr<Integer>> rs({r});
  vector<shared_ptr<Integer>> ret;
  commit(ms, rs, ret);
  return ret[0];
}

void PolynomialCommitment::commit(
    const shared_ptr<Matrix> &ms,
    const vector<shared_ptr<Integer>> &rs,
    vector<shared_ptr<Integer>> &ret)
{
  if (gi.size() < ms->n + 1)
    throw invalid_argument("number of generators and messages do not match");

  for (size_t i = 0; i < ms->m; i++)
  {
    auto c = gi[0]->modPow(rs[i], Q); // c = g^ri

    // c = g^ri * g0^m0 * g1^m1 * ...
    if (ms->rowExists(i))
    {
      for (auto it : ms->values[i]) // skip mi = 0, where gi^0 = 1
      {
        size_t j = it.first;
        auto v = it.second;

        if (v->eq(Integer::ZERO()))
          continue;

        auto gm = gi[j + 1];
        if (!v->eq(Integer::ONE()))
        {
          gm = gm->modPow(v, Q);
        }
        c = c->modMul(gm, Q);
      }
    }
    ret.push_back(c);
  }
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
  vector<shared_ptr<Integer>> u;
  if (n > 1)
  {
    u = Random::getRandoms(n - 1, p, true);
  }
  u.push_back(Integer::ZERO());
  T->appendRow(u);

  // mask vector t0'
  for (size_t i = 0; i < n - 1; i++)
  {
    auto v = T->cell(m1, i + 1)->sub(u[i])->mod(p);
    T->cell(m1, i + 1, v);
  }
  return T;
}

void PolynomialCommitment::commit(
    size_t m1, size_t m2, size_t n,
    const shared_ptr<Matrix> &T,
    vector<shared_ptr<Integer>> &ri,
    vector<shared_ptr<Integer>> &ret)
{
  size_t m = m1 + m2 + 1;
  if (T->m != m || T->n != n)
    throw invalid_argument("m1, m2, n do not match with the dimension of matrix T");
  if (ri.size() != 0 && ri.size() != m)
    throw invalid_argument("ri.size() do not match (m1 + m2 + 1)");

  // Generate randomness if it isn't provided
  if (ri.size() == 0)
  {
    auto rs = Random::getRandoms(m, p, true);
    ri.insert(ri.begin(), rs.begin(), rs.end());
  }

  // calculate commitment
  commit(T, ri, ret);
}

void PolynomialCommitment::eval(
    size_t m1, size_t m2, size_t n,
    const shared_ptr<Matrix> &T,
    const vector<shared_ptr<Integer>> &ri,
    const shared_ptr<Integer> &x,
    vector<shared_ptr<Integer>> &ret)
{
  size_t m = m1 + m2 + 1;
  if (T->m != m || T->n != n)
    throw invalid_argument("m1, m2, n do not match with the dimension of matrix T");
  if (ri.size() != 0 && ri.size() != m)
    throw invalid_argument("ri.size() do not match (m1 + m2 + 1)");

  // Compute Z(X)
  auto xn = x->modPow(make_shared<IntegerImpl>(n), p);
  vector<shared_ptr<Integer>> xns; // [1, x^n, x^2n, ... , x^m1n or x^(m2-1)n]
  Matrix::powerVector(xn, max(m1 + 1, m2))->row(0, xns);
  vector<shared_ptr<Integer>> zx;
  for (size_t i = m1; i > 0; i--)
  {
    auto z = xns[i]->inv(p);
    zx.push_back(z);
  }
  if (m2 > 0)
  {
    zx.push_back(x);
  }
  for (size_t i = 1; i < m2; i++)
  {
    auto z = xns[i]->modMul(x, p);
    zx.push_back(z);
  }
  zx.push_back(x->modMul(x, p));
  auto Z = make_shared<Matrix>(zx);

  // t_ = Z * T
  auto t_ = Z->mul(T, p);

  // r_ = Z * r
  auto r_ = make_shared<Matrix>(ri);
  r_ = r_->t();

  r_ = Z->mul(r_, p);

  // return result vector
  t_->row(0, ret);
  ret.push_back(r_->cell(0, 0));
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
  return v->cell(0, 0);
}
