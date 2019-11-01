#include "./PolynomialCommitment.hpp"

using namespace polyu;

PolynomialCommitment::PolynomialCommitment(
    const shared_ptr<Integer> &N,
    const vector<shared_ptr<Integer>> &gi)
{
  this->N = N;
  this->gi.insert(this->gi.begin(), gi.begin(), gi.end());
}

shared_ptr<Integer> PolynomialCommitment::commit(
    const vector<shared_ptr<Integer>> &mi,
    const shared_ptr<Integer> &r)
{
  if (gi.size() < mi.size() + 1)
    throw invalid_argument("number of generators and messages do not match");

  auto ret = gi[0]->modPow(r, N);

  for (size_t i = 0; i < mi.size(); i++)
  {
    auto gm = gi[i + 1]->modPow(mi[i], N);
    ret = ret->modMul(gm, N);
  }

  return ret;
}
