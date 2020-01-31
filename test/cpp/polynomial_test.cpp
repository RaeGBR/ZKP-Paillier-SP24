#include "gtest/gtest.h"

#include "lib/math/Polynomial.hpp"
#include "lib/math/Matrix.hpp"
#include "lib/math/IntegerImpl.hpp"

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

TEST(Polynomial, Constructor_and_get)
{
  vector<shared_ptr<Matrix>> values;
  values.push_back(make_shared<Matrix>(vector<int>({1, 2})));
  values.push_back(make_shared<Matrix>(vector<int>({3, 4})));
  values.push_back(make_shared<Matrix>(vector<int>({5, 6})));
  values.push_back(make_shared<Matrix>(vector<int>({7, 8})));

  auto poly = make_shared<Polynomial>(values, -1);
  EXPECT_EQ(poly->getSmallestDegree(), -1);
  EXPECT_EQ(poly->getLargestDegree(), 2);
  EXPECT_EQ(poly->get(-2)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(-1)->toString(), "[[\"1\",\"2\"]]");
  EXPECT_EQ(poly->get(0)->toString(), "[[\"3\",\"4\"]]");
  EXPECT_EQ(poly->get(1)->toString(), "[[\"5\",\"6\"]]");
  EXPECT_EQ(poly->get(2)->toString(), "[[\"7\",\"8\"]]");
  EXPECT_EQ(poly->get(3)->toString(), "[[\"0\",\"0\"]]");
}

TEST(Polynomial, Clone)
{
  vector<shared_ptr<Matrix>> values;
  values.push_back(make_shared<Matrix>(vector<int>({1, 2})));
  values.push_back(make_shared<Matrix>(vector<int>({3, 4})));
  values.push_back(make_shared<Matrix>(vector<int>({5, 6})));
  values.push_back(make_shared<Matrix>(vector<int>({7, 8})));

  auto poly = make_shared<Polynomial>(values, -1);
  auto poly2 = poly->clone();
  EXPECT_EQ(poly2->getSmallestDegree(), -1);
  EXPECT_EQ(poly2->getLargestDegree(), 2);
  EXPECT_EQ(poly2->get(-2)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly2->get(-1)->toString(), "[[\"1\",\"2\"]]");
  EXPECT_EQ(poly2->get(0)->toString(), "[[\"3\",\"4\"]]");
  EXPECT_EQ(poly2->get(1)->toString(), "[[\"5\",\"6\"]]");
  EXPECT_EQ(poly2->get(2)->toString(), "[[\"7\",\"8\"]]");
  EXPECT_EQ(poly2->get(3)->toString(), "[[\"0\",\"0\"]]");
}

TEST(Polynomial, Put_and_erase)
{
  vector<shared_ptr<Matrix>> values;
  values.push_back(make_shared<Matrix>(vector<int>({1, 2})));
  values.push_back(make_shared<Matrix>(vector<int>({3, 4})));
  values.push_back(make_shared<Matrix>(vector<int>({5, 6})));
  values.push_back(make_shared<Matrix>(vector<int>({7, 8})));

  auto poly = make_shared<Polynomial>(values, -1);
  EXPECT_EQ(poly->getSmallestDegree(), -1);
  EXPECT_EQ(poly->getLargestDegree(), 2);

  poly->put(4, make_shared<Matrix>(vector<int>({9, 10})));
  EXPECT_EQ(poly->getSmallestDegree(), -1);
  EXPECT_EQ(poly->getLargestDegree(), 4);
  EXPECT_EQ(poly->get(-2)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(-1)->toString(), "[[\"1\",\"2\"]]");
  EXPECT_EQ(poly->get(0)->toString(), "[[\"3\",\"4\"]]");
  EXPECT_EQ(poly->get(1)->toString(), "[[\"5\",\"6\"]]");
  EXPECT_EQ(poly->get(2)->toString(), "[[\"7\",\"8\"]]");
  EXPECT_EQ(poly->get(3)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(4)->toString(), "[[\"9\",\"10\"]]");
  EXPECT_EQ(poly->get(5)->toString(), "[[\"0\",\"0\"]]");

  poly->erase(1);
  EXPECT_EQ(poly->getSmallestDegree(), -1);
  EXPECT_EQ(poly->getLargestDegree(), 4);
  EXPECT_EQ(poly->get(-2)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(-1)->toString(), "[[\"1\",\"2\"]]");
  EXPECT_EQ(poly->get(0)->toString(), "[[\"3\",\"4\"]]");
  EXPECT_EQ(poly->get(1)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(2)->toString(), "[[\"7\",\"8\"]]");
  EXPECT_EQ(poly->get(3)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(4)->toString(), "[[\"9\",\"10\"]]");
  EXPECT_EQ(poly->get(5)->toString(), "[[\"0\",\"0\"]]");

  poly->erase(-1);
  EXPECT_EQ(poly->getSmallestDegree(), 0);
  EXPECT_EQ(poly->getLargestDegree(), 4);
  EXPECT_EQ(poly->get(-2)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(-1)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(0)->toString(), "[[\"3\",\"4\"]]");
  EXPECT_EQ(poly->get(1)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(2)->toString(), "[[\"7\",\"8\"]]");
  EXPECT_EQ(poly->get(3)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(4)->toString(), "[[\"9\",\"10\"]]");
  EXPECT_EQ(poly->get(5)->toString(), "[[\"0\",\"0\"]]");

  poly->erase(4);
  EXPECT_EQ(poly->getSmallestDegree(), 0);
  EXPECT_EQ(poly->getLargestDegree(), 2);
  EXPECT_EQ(poly->get(-2)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(-1)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(0)->toString(), "[[\"3\",\"4\"]]");
  EXPECT_EQ(poly->get(1)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(2)->toString(), "[[\"7\",\"8\"]]");
  EXPECT_EQ(poly->get(3)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(4)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(5)->toString(), "[[\"0\",\"0\"]]");

  poly->erase(5);
  EXPECT_EQ(poly->getSmallestDegree(), 0);
  EXPECT_EQ(poly->getLargestDegree(), 2);
  EXPECT_EQ(poly->get(-2)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(-1)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(0)->toString(), "[[\"3\",\"4\"]]");
  EXPECT_EQ(poly->get(1)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(2)->toString(), "[[\"7\",\"8\"]]");
  EXPECT_EQ(poly->get(3)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(4)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(poly->get(5)->toString(), "[[\"0\",\"0\"]]");

  auto degrees = poly->getDegrees();
  EXPECT_EQ(degrees.size(), 2);
  EXPECT_EQ(degrees[0], 0);
  EXPECT_EQ(degrees[1], 2);
}

TEST(Polynomial, Add)
{
  auto p1 = make_shared<Polynomial>();
  p1->put(-1, make_shared<Matrix>(vector<int>({1})));
  p1->put(1, make_shared<Matrix>(vector<int>({2})));
  p1->put(2, make_shared<Matrix>(vector<int>({3})));

  auto p2 = make_shared<Polynomial>();
  p2->put(-2, make_shared<Matrix>(vector<int>({4})));
  p2->put(-1, make_shared<Matrix>(vector<int>({5})));
  p2->put(1, make_shared<Matrix>(vector<int>({6})));

  auto ret = p1->add(p2);
  EXPECT_EQ(ret->getSmallestDegree(), -2);
  EXPECT_EQ(ret->getLargestDegree(), 2);
  EXPECT_EQ(ret->get(-3)->toString(), "[[\"0\"]]");
  EXPECT_EQ(ret->get(-2)->toString(), "[[\"4\"]]");
  EXPECT_EQ(ret->get(-1)->toString(), "[[\"6\"]]");
  EXPECT_EQ(ret->get(0)->toString(), "[[\"0\"]]");
  EXPECT_EQ(ret->get(1)->toString(), "[[\"8\"]]");
  EXPECT_EQ(ret->get(2)->toString(), "[[\"3\"]]");
  EXPECT_EQ(ret->get(3)->toString(), "[[\"0\"]]");
}

TEST(Polynomial, Mul_with_integer)
{
  auto p1 = make_shared<Polynomial>();
  p1->put(-1, make_shared<Matrix>(vector<int>({1, 2})));
  p1->put(1, make_shared<Matrix>(vector<int>({3, 4})));

  auto m = make_shared<IntegerImpl>(2);
  auto ret = p1->mul(m);
  EXPECT_EQ(ret->getSmallestDegree(), -1);
  EXPECT_EQ(ret->getLargestDegree(), 1);
  EXPECT_EQ(ret->get(-2)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(ret->get(-1)->toString(), "[[\"2\",\"4\"]]");
  EXPECT_EQ(ret->get(0)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(ret->get(1)->toString(), "[[\"6\",\"8\"]]");
  EXPECT_EQ(ret->get(2)->toString(), "[[\"0\",\"0\"]]");
}

TEST(Polynomial, inner_product_with_matrix)
{
  auto p1 = make_shared<Polynomial>();
  p1->put(-1, make_shared<Matrix>(vector<int>({1, 2})));
  p1->put(1, make_shared<Matrix>(vector<int>({3, 4})));

  auto m = make_shared<Matrix>(vector<int>({5, 6}));
  auto ret = p1->inner(m);
  EXPECT_EQ(ret->getSmallestDegree(), -1);
  EXPECT_EQ(ret->getLargestDegree(), 1);
  EXPECT_EQ(ret->get(-2)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(ret->get(-1)->toString(), "[[\"5\",\"12\"]]");
  EXPECT_EQ(ret->get(0)->toString(), "[[\"0\",\"0\"]]");
  EXPECT_EQ(ret->get(1)->toString(), "[[\"15\",\"24\"]]");
  EXPECT_EQ(ret->get(2)->toString(), "[[\"0\",\"0\"]]");
}

TEST(Polynomial, Mul_with_poly)
{
  auto p1 = make_shared<Polynomial>();
  p1->put(-1, make_shared<Matrix>(vector<int>({1, 2})));
  p1->put(1, make_shared<Matrix>(vector<int>({3, 4})));

  auto p2 = make_shared<Polynomial>();
  p2->put(-1, make_shared<Matrix>(vector<int>({5, 6})));
  p2->put(2, make_shared<Matrix>(vector<int>({7, 8})));
  p2 = p2->t();

  auto ret = p1->mul(p2);
  EXPECT_EQ(ret->getSmallestDegree(), -2);
  EXPECT_EQ(ret->getLargestDegree(), 3);
  EXPECT_EQ(ret->get(-3)->toString(), "[[\"0\"]]");
  EXPECT_EQ(ret->get(-2)->toString(), "[[\"17\"]]");
  EXPECT_EQ(ret->get(-1)->toString(), "[[\"0\"]]");
  EXPECT_EQ(ret->get(1)->toString(), "[[\"23\"]]");
  EXPECT_EQ(ret->get(0)->toString(), "[[\"39\"]]");
  EXPECT_EQ(ret->get(2)->toString(), "[[\"0\"]]");
  EXPECT_EQ(ret->get(3)->toString(), "[[\"53\"]]");
  EXPECT_EQ(ret->get(4)->toString(), "[[\"0\"]]");
}

TEST(Polynomial, Eval)
{
  // f(x) = 1 + 2 * x + 3 * x^2
  auto p = make_shared<Polynomial>();
  p->put(0, make_shared<Matrix>(vector<int>({1})));
  p->put(1, make_shared<Matrix>(vector<int>({2})));
  p->put(2, make_shared<Matrix>(vector<int>({3})));

  EXPECT_EQ(p->eval(make_shared<IntegerImpl>(2))->toString(), "[[\"17\"]]");
  EXPECT_EQ(p->eval(make_shared<IntegerImpl>(3))->toString(), "[[\"34\"]]");
}

TEST(Polynomial, Eval_with_mod)
{
  auto modulus = make_shared<IntegerImpl>(7);

  // f(x) = 1 + x + 2 * x^2
  auto p = make_shared<Polynomial>();
  p->put(0, make_shared<Matrix>(vector<int>({1})));
  p->put(1, make_shared<Matrix>(vector<int>({2})));
  p->put(2, make_shared<Matrix>(vector<int>({3})));

  EXPECT_EQ(p->eval(make_shared<IntegerImpl>(2), modulus)->toString(), "[[\"3\"]]");
  EXPECT_EQ(p->eval(make_shared<IntegerImpl>(3), modulus)->toString(), "[[\"6\"]]");
}

TEST(Polynomial, Eval_with_mod_and_negative_degree)
{
  auto modulus = make_shared<IntegerImpl>(7);

  // f(x) = x^-1 + 2 + 3 * x + 4 * x^2
  auto p = make_shared<Polynomial>();
  p->put(-1, make_shared<Matrix>(vector<int>({1})));
  p->put(0, make_shared<Matrix>(vector<int>({2})));
  p->put(1, make_shared<Matrix>(vector<int>({3})));
  p->put(2, make_shared<Matrix>(vector<int>({4})));

  EXPECT_EQ(p->eval(make_shared<IntegerImpl>(2), modulus)->toString(), "[[\"0\"]]");
  EXPECT_EQ(p->eval(make_shared<IntegerImpl>(3), modulus)->toString(), "[[\"3\"]]");
}

TEST(Polynomial, Eval_matrix)
{
  auto modulus = make_shared<IntegerImpl>(7);

  // f(x) = x^-1 + 2 + 3 * x + 4 * x^2
  auto p = make_shared<Polynomial>();
  p->put(-1, make_shared<Matrix>(vector<int>({0, 1, 2, 3}))->group(2));
  p->put(0, make_shared<Matrix>(vector<int>({0, 0, 1, 2}))->group(2));
  p->put(1, make_shared<Matrix>(vector<int>({0, 0, 0, 1}))->group(2));
  p->put(2, make_shared<Matrix>(vector<int>({1, 1, 1, 1}))->group(2));

  EXPECT_EQ(p->eval(make_shared<IntegerImpl>(2), modulus)->toString(), "[[\"4\",\"1\"],[\"6\",\"6\"]]");
  EXPECT_EQ(p->eval(make_shared<IntegerImpl>(3), modulus)->toString(), "[[\"2\",\"0\"],[\"6\",\"1\"]]");
}

} // namespace
