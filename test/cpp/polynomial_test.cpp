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
TEST(Polynomial, Matrix_Construction) {
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

  auto zero = make_shared<Matrix>(1, 1);

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
  // EXPECT_EQ(obj[0]["matrix"].dump(), 
  
  EXPECT_EQ(obj[1]["degree"], 0);
  EXPECT_EQ(obj[1]["matrix"].dump(), "[[\"0\"]]");

  EXPECT_EQ(obj[2]["degree"], 1);
  EXPECT_EQ(obj[2]["matrix"].dump(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");

  EXPECT_EQ(obj[3]["degree"], 2);
  EXPECT_EQ(obj[3]["matrix"].dump(), "[[\"0\"]]");

  EXPECT_EQ(obj[4]["degree"], 3);
  EXPECT_EQ(obj[4]["matrix"].dump(), "[[\"0\",\"1\",\"2\"],[\"0\",\"1\",\"2\"]]");
}

// Polynomial Cloning

// Get Term

// Put Term

// Erase Term

// Add

// Scalar Mul

// Cross Prod.

// Dot Prod.

// Mul. Poly

}