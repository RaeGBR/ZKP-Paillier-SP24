#include "gtest/gtest.h"

#include "lib/math/Polynomial.hpp"
#include "lib/math/Matrix.hpp"

using namespace std;
using namespace cryptoplus;

namespace
{

TEST(Polynomial, Default)
{
  auto a = make_shared<Polynomial>();
  EXPECT_EQ(a->getSmallestDegree(), 0);
  EXPECT_EQ(a->getLargestDegree(), 0);
}

// Construct from Matrix
TEST(Polynomial, Matrix_Construction_and_Get) {
  const size_t m = 2;
  const size_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*a)[i][j] = Integer::createWithString(to_string(i * n + j));
    }
  }
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  
  auto b = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*b)[i][j] = Integer::createWithString(to_string(j));
    }
  }
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"0\",\"1\",\"2\"]]");

  auto zero = make_shared<Matrix>(1, 1);
  EXPECT_EQ(zero->toString(), "[[\"0\"]]");

  // p(x) = b * x^3 + 0 * x^2 + a * x^1 + 0 + a * x^-1
  vector<shared_ptr<Matrix>> coefficients;
  coefficients.push_back(b);    // 3
  coefficients.push_back(zero); // 2
  coefficients.push_back(a);    // 1
  coefficients.push_back(zero); // 0
  coefficients.push_back(a);    // -1
  
  auto poly = make_shared<Polynomial>(coefficients, -1);
  EXPECT_EQ(poly->getSmallestDegree(), -1);
  EXPECT_EQ(poly->values.size(), 5);
  EXPECT_EQ(poly->lsd + poly->values.size() - 1, 3);
  EXPECT_EQ(poly->getLargestDegree(), 3);
  
  auto obj = poly->toJson();
  EXPECT_EQ(obj[0]["degree"], -1);
  EXPECT_EQ(obj[0]["matrix"].dump(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(poly->get(-1)->toString(), obj[0]["matrix"].dump());
  
  EXPECT_EQ(obj[1]["degree"], 0);
  EXPECT_EQ(obj[1]["matrix"].dump(), "[[\"0\"]]");
  EXPECT_EQ(poly->get(0)->toString(), obj[1]["matrix"].dump());

  EXPECT_EQ(obj[2]["degree"], 1);
  EXPECT_EQ(obj[2]["matrix"].dump(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(poly->get(1)->toString(), obj[2]["matrix"].dump());

  EXPECT_EQ(obj[3]["degree"], 2);
  EXPECT_EQ(obj[3]["matrix"].dump(), "[[\"0\"]]");
  EXPECT_EQ(poly->get(2)->toString(), obj[3]["matrix"].dump());

  EXPECT_EQ(obj[4]["degree"], 3);
  EXPECT_EQ(obj[4]["matrix"].dump(), "[[\"0\",\"1\",\"2\"],[\"0\",\"1\",\"2\"]]");
  EXPECT_EQ(poly->get(3)->toString(), obj[4]["matrix"].dump());
}

TEST(Polynomial, PUT_and_Erase) {
  const size_t m = 2;
  const size_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*a)[i][j] = Integer::createWithString(to_string(i * n + j));
    }
  }
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  
  auto b = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*b)[i][j] = Integer::createWithString(to_string(j));
    }
  }
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"0\",\"1\",\"2\"]]");

  auto zero = make_shared<Matrix>(1, 1);
  EXPECT_EQ(zero->toString(), "[[\"0\"]]");

  auto poly = make_shared<Polynomial>();
  // Add Greater
  poly->put(a, 1);
  poly->put(a, 4);

  EXPECT_EQ(poly->getSmallestDegree(), 1);
  EXPECT_EQ(poly->getLargestDegree(), 4);
  EXPECT_EQ(poly->get(0), nullptr);
  EXPECT_EQ(poly->get(1)->toString(), a->toString());
  EXPECT_EQ(poly->get(2)->toString(), zero->toString());
  EXPECT_EQ(poly->get(3)->toString(), zero->toString());
  EXPECT_EQ(poly->get(4)->toString(), a->toString());

  // Add Smaller
  poly->put(b, -1);
  poly->put(b, -3);

  EXPECT_EQ(poly->getSmallestDegree(), -3);
  EXPECT_EQ(poly->getLargestDegree(), 4);
  EXPECT_EQ(poly->get(-1)->toString(), b->toString());
  EXPECT_EQ(poly->get(-2)->toString(), zero->toString());
  EXPECT_EQ(poly->get(-3)->toString(), b->toString());

  // Add Within
  poly->put(a, 0);
  EXPECT_EQ(poly->get(0)->toString(), a->toString());

  // Erase Out of Bound
  bool success = poly->erase(-4);
  EXPECT_FALSE(success);

  // Erase existing degree
  success = poly->erase(-3);
  EXPECT_TRUE(success);
  // Erase will delete all zeros if deleting the smallest degree
  EXPECT_EQ(poly->getSmallestDegree(), -1);

  // Erase existing degree

  success = poly->erase(4);
  EXPECT_TRUE(success);
  // Erase will delete all zeros if deleting the smallest degree
  EXPECT_EQ(poly->getLargestDegree(), 1);

  // Erase existing degree
  success = poly->erase(1);
  EXPECT_TRUE(success);
  success = poly->erase(-1);
  EXPECT_TRUE(success);
  success = poly->erase(0);
  EXPECT_TRUE(success);
  // When all coefficients are zero -> values[0] will init as zero
  EXPECT_EQ(poly->getLargestDegree(), 0);
  EXPECT_EQ(poly->getSmallestDegree(), 0);
  EXPECT_EQ(poly->get(0)->toString(), zero->toString());
}


TEST(Polynomial, Clone) {
  const size_t m = 2;
  const size_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*a)[i][j] = Integer::createWithString(to_string(i * n + j));
    }
  }
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  
  auto b = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*b)[i][j] = Integer::createWithString(to_string(j));
    }
  }
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"0\",\"1\",\"2\"]]");

  // b * x^3 + a * x^1
  auto poly = make_shared<Polynomial>();
  poly->put(a, 1);
  poly->put(b, 3);
  EXPECT_EQ(poly->getSmallestDegree(), 1);
  EXPECT_EQ(poly->getLargestDegree(), 3);
  EXPECT_EQ(poly->get(1)->toString(), a->toString());
  EXPECT_EQ(poly->get(2)->toString(), "[[\"0\"]]");
  EXPECT_EQ(poly->get(3)->toString(), b->toString());

  auto pB = poly->clone();
  EXPECT_EQ(pB->getSmallestDegree(), 1);
  EXPECT_EQ(pB->getLargestDegree(), 3);
  EXPECT_EQ(pB->get(1)->toString(), a->toString());
  EXPECT_EQ(pB->get(2)->toString(), "[[\"0\"]]");
  EXPECT_EQ(pB->get(3)->toString(), b->toString());

  // Test will change of pB affect the origin polynomial
  pB->erase(2);
  pB->erase(1);
  pB->erase(3);

  EXPECT_EQ(pB->getLargestDegree(), 0);
  EXPECT_EQ(pB->getSmallestDegree(), 0);

  auto zero = make_shared<Matrix>(1, 1);
  EXPECT_EQ(zero->toString(), "[[\"0\"]]");
  EXPECT_EQ(pB->get(0)->toString(), zero->toString());
}

// Add

// Scalar Mul

// Cross Prod.

// Dot Prod.

// Mul. Poly

}