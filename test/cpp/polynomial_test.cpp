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
TEST(Polynomial, Add) {
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

  // p(a) = b * x^3 + a * x^1
  auto pa = make_shared<Polynomial>();
  pa->put(a, 1);
  pa->put(b, 3);

  // p(b) = a * x^2 + b * x^1
  auto pb = make_shared<Polynomial>();
  pb->put(b, 1);
  pb->put(a, 2);

  // p(s) = b * x^3 + a * x^2 + (a+b) * x^1
  auto ps = pa->add(pb);

  EXPECT_EQ(ps->getLargestDegree(), 3);
  EXPECT_EQ(ps->getSmallestDegree(), 1);
  EXPECT_EQ(ps->get(3)->toString(), b->toString());
  EXPECT_EQ(ps->get(2)->toString(), a->toString());
  EXPECT_EQ(ps->get(1)->toString(), "[[\"0\",\"2\",\"4\"],[\"3\",\"5\",\"7\"]]");

  // Try to add with Negative Coefficients
  pa->put(a, -3);
  pb->put(a, -3);
  ps = pa->add(pb);

  EXPECT_EQ(ps->getLargestDegree(), 3);
  EXPECT_EQ(ps->getSmallestDegree(), -3);
  EXPECT_EQ(ps->get(3)->toString(), b->toString());
  EXPECT_EQ(ps->get(2)->toString(), a->toString());
  EXPECT_EQ(ps->get(1)->toString(), "[[\"0\",\"2\",\"4\"],[\"3\",\"5\",\"7\"]]");
  EXPECT_EQ(ps->get(0)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-1)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-2)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-3)->toString(), "[[\"0\",\"2\",\"4\"],[\"6\",\"8\",\"10\"]]");

  // Try with Modulus
  ps = pa->add(pb, Integer::createWithNumber(5));

  EXPECT_EQ(ps->getLargestDegree(), 3);
  EXPECT_EQ(ps->getSmallestDegree(), -3);
  EXPECT_EQ(ps->get(3)->toString(), b->toString());
  EXPECT_EQ(ps->get(2)->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"0\"]]");
  EXPECT_EQ(ps->get(1)->toString(), "[[\"0\",\"2\",\"4\"],[\"3\",\"0\",\"2\"]]");
  EXPECT_EQ(ps->get(0)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-1)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-2)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-3)->toString(), "[[\"0\",\"2\",\"4\"],[\"1\",\"3\",\"0\"]]");
}

// Scalar Mul
TEST(Polynomial, Scalar_Multiplication)
{
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

  // p(a) = b * x^3 + a * x^1
  auto pa = make_shared<Polynomial>();
  pa->put(a, 1);
  pa->put(b, 3);

  // p(s) = 5b * x^3 + 5a * x^1
  auto ps = pa->mul(Integer::createWithNumber(5));
  EXPECT_EQ(ps->getLargestDegree(), 3);
  EXPECT_EQ(ps->getSmallestDegree(), 1);
  EXPECT_EQ(ps->get(3)->toString(), "[[\"0\",\"5\",\"10\"],[\"0\",\"5\",\"10\"]]");
  EXPECT_EQ(ps->get(2)->toString(), zero->toString());
  EXPECT_EQ(ps->get(1)->toString(), "[[\"0\",\"5\",\"10\"],[\"15\",\"20\",\"25\"]]");

  // try with modulus
  ps = pa->mul(Integer::createWithNumber(5), Integer::createWithNumber(12));
  EXPECT_EQ(ps->getLargestDegree(), 3);
  EXPECT_EQ(ps->getSmallestDegree(), 1);
  EXPECT_EQ(ps->get(3)->toString(), "[[\"0\",\"5\",\"10\"],[\"0\",\"5\",\"10\"]]");
  EXPECT_EQ(ps->get(2)->toString(), zero->toString());
  EXPECT_EQ(ps->get(1)->toString(), "[[\"0\",\"5\",\"10\"],[\"3\",\"8\",\"1\"]]");

  // try with negative coefficient
  // p(a) = b * x^3 + a * x^1 + (b+a) * x^-2
  pa->put(b->add(a), -2);

  ps = pa->mul(Integer::createWithNumber(5));
  EXPECT_EQ(ps->getLargestDegree(), 3);
  EXPECT_EQ(ps->getSmallestDegree(), -2);
  EXPECT_EQ(ps->get(3)->toString(), "[[\"0\",\"5\",\"10\"],[\"0\",\"5\",\"10\"]]");
  EXPECT_EQ(ps->get(2)->toString(), zero->toString());
  EXPECT_EQ(ps->get(1)->toString(), "[[\"0\",\"5\",\"10\"],[\"15\",\"20\",\"25\"]]");
  EXPECT_EQ(ps->get(0)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-1)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-2)->toString(), "[[\"0\",\"10\",\"20\"],[\"15\",\"25\",\"35\"]]");

  // with modulus
  ps = pa->mul(Integer::createWithNumber(5), Integer::createWithNumber(12));
  EXPECT_EQ(ps->getLargestDegree(), 3);
  EXPECT_EQ(ps->getSmallestDegree(), -2);
  EXPECT_EQ(ps->get(3)->toString(), "[[\"0\",\"5\",\"10\"],[\"0\",\"5\",\"10\"]]");
  EXPECT_EQ(ps->get(2)->toString(), zero->toString());
  EXPECT_EQ(ps->get(1)->toString(), "[[\"0\",\"5\",\"10\"],[\"3\",\"8\",\"1\"]]");
  EXPECT_EQ(ps->get(0)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-1)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-2)->toString(), "[[\"0\",\"10\",\"8\"],[\"3\",\"1\",\"11\"]]");
  
}

// Cross Prod.
TEST(Polynomial, Cross_Product)
{
  const size_t m = 2;
  const size_t n = 3;
  const size_t p = 2;
  auto a = make_shared<Matrix>(m, n);
  auto b = make_shared<Matrix>(n, p);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*a)[i][j] = Integer::createWithString(to_string(i * n + j + 1));
      for (size_t k = 0; k < p; k++)
      {
        (*b)[j][k] = Integer::create(to_string(m * n + j * p + k + 1), 10);
      }
    }
  }

  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]");
  EXPECT_EQ(b->toString(), "[[\"7\",\"8\"],[\"9\",\"10\"],[\"11\",\"12\"]]");

  auto zero = make_shared<Matrix>(1, 1);
  EXPECT_EQ(zero->toString(), "[[\"0\"]]");

  // p(a) = a * x^2  + a * x^-3
  auto pa = make_shared<Polynomial>();
  pa->put(a, -2);
  pa->put(a, 2);

  auto ps = pa->mul(b);
  EXPECT_EQ(ps->get(2)->toString(), "[[\"58\",\"64\"],[\"139\",\"154\"]]");
  EXPECT_EQ(ps->get(1)->toString(), zero->toString());
  EXPECT_EQ(ps->get(0)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-1)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-2)->toString(), "[[\"58\",\"64\"],[\"139\",\"154\"]]");

  // try with modulus
  ps = pa->mul(b, Integer::createWithNumber(7));
  EXPECT_EQ(ps->get(2)->toString(), "[[\"2\",\"1\"],[\"6\",\"0\"]]");
  EXPECT_EQ(ps->get(1)->toString(), zero->toString());
  EXPECT_EQ(ps->get(0)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-1)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-2)->toString(), "[[\"2\",\"1\"],[\"6\",\"0\"]]");
}

// Dot Prod.
TEST(Polynomial, Dot_Product)
{
  const size_t m = 2;
  const size_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  auto b = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*a)[i][j] = Integer::create(to_string(i * n + j), 10);
      (*b)[i][j] = Integer::create(to_string(i * n + j + 1), 10);
    }
  }
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]");

  auto zero = make_shared<Matrix>(1, 1);
  EXPECT_EQ(zero->toString(), "[[\"0\"]]");

   // p(a) = a * x^2  + a * x^-3
  auto pa = make_shared<Polynomial>();
  pa->put(a, -2);
  pa->put(a, 2);

  auto ps = pa->dot(b);
  EXPECT_EQ(ps->get(2)->toString(), "[[\"8\",\"17\"],[\"26\",\"62\"]]");
  EXPECT_EQ(ps->get(1)->toString(), zero->toString());
  EXPECT_EQ(ps->get(0)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-1)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-2)->toString(), "[[\"8\",\"17\"],[\"26\",\"62\"]]");

  // try with modulus
  ps = pa->dot(b, Integer::createWithNumber(7));
  EXPECT_EQ(ps->get(2)->toString(),  "[[\"1\",\"3\"],[\"5\",\"6\"]]");
  EXPECT_EQ(ps->get(1)->toString(), zero->toString());
  EXPECT_EQ(ps->get(0)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-1)->toString(), zero->toString());
  EXPECT_EQ(ps->get(-2)->toString(),  "[[\"1\",\"3\"],[\"5\",\"6\"]]");
}

// Mul. Poly

}