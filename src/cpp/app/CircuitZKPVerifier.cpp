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
}

void CircuitZKPVerifier::setCommits(const vector<shared_ptr<Integer>> &commits)
{
  if (commits.size() != 3 * m + 1)
    throw invalid_argument("commitments count is not correct");

  commitA.clear();
  commitB.clear();
  commitC.clear();
  for (size_t i = 0; i < m; i++)
    commitA.push_back(commits[i]);
  for (size_t i = 0; i < m; i++)
    commitB.push_back(commits[m + i]);
  for (size_t i = 0; i < m; i++)
    commitC.push_back(commits[2 * m + i]);
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
