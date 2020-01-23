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
