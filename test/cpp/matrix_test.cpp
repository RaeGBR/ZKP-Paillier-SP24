#include "gtest/gtest.h"

#include "lib/math/Matrix.hpp"

using namespace std;
using namespace cryptoplus;

namespace
{

TEST(Matrix, Default)
{
  const uint32_t m = 2;
  const uint32_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  EXPECT_EQ(a->m, m);
  EXPECT_EQ(a->n, n);
  for (uint32_t i = 0; i < m; i++)
  {
    for (uint32_t j = 0; j < n; j++)
    {
      EXPECT_EQ(a->values[i][j]->toString(), "0");
    }
  }

  EXPECT_EQ(a->toString(), "[[\"0\",\"0\",\"0\"],[\"0\",\"0\",\"0\"]]");
}

TEST(Matrix, Identity)
{
  const uint32_t size = 3;
  auto a = Matrix::identity(size);
  EXPECT_EQ(a->m, size);
  EXPECT_EQ(a->n, size);
  for (uint32_t i = 0; i < size; i++)
  {
    for (uint32_t j = 0; j < size; j++)
    {
      string expected = i == j ? "1" : "0";
      EXPECT_EQ(a->values[i][j]->toString(), expected);
    }
  }

  EXPECT_EQ(a->toString(), "[[\"1\",\"0\",\"0\"],[\"0\",\"1\",\"0\"],[\"0\",\"0\",\"1\"]]");
}

TEST(Matrix, Clone)
{
  const uint32_t m = 2;
  const uint32_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  for (uint32_t i = 0; i < m; i++)
  {
    for (uint32_t j = 0; j < n; j++)
    {
      a->values[i][j] = Integer::createWithString(to_string(i * n + j));
    }
  }

  auto b = make_shared<Matrix>(a->values);
  auto c = b->clone();
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(c->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");

  c->values[0][0] = Integer::createWithString("9");
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(c->toString(), "[[\"9\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
}

TEST(Matrix, Tansposition)
{
  const uint32_t m = 2;
  const uint32_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  for (uint32_t i = 0; i < m; i++)
  {
    for (uint32_t j = 0; j < n; j++)
    {
      a->values[i][j] = Integer::createWithString(to_string(i * n + j));
    }
  }

  auto b = a->t();
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"0\",\"3\"],[\"1\",\"4\"],[\"2\",\"5\"]]");
}

TEST(Matrix, Add)
{
  const uint32_t m = 2;
  const uint32_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  auto b = make_shared<Matrix>(m, n);
  for (uint32_t i = 0; i < m; i++)
  {
    for (uint32_t j = 0; j < n; j++)
    {
      a->values[i][j] = Integer::createWithString(to_string(i * n + j));
      b->values[i][j] = Integer::createWithString(to_string(i + j));
    }
  }

  auto c = a->add(b);
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"1\",\"2\",\"3\"]]");
  EXPECT_EQ(c->toString(), "[[\"0\",\"2\",\"4\"],[\"4\",\"6\",\"8\"]]");
}

TEST(Matrix, ModAdd)
{
  const uint32_t m = 2;
  const uint32_t n = 3;
  const auto modulus = Integer::createWithString("6");
  auto a = make_shared<Matrix>(m, n);
  auto b = make_shared<Matrix>(m, n);
  for (uint32_t i = 0; i < m; i++)
  {
    for (uint32_t j = 0; j < n; j++)
    {
      a->values[i][j] = Integer::createWithString(to_string(i * n + j));
      b->values[i][j] = Integer::createWithString(to_string(i + j));
    }
  }

  auto c = a->add(b, modulus);
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"1\",\"2\",\"3\"]]");
  EXPECT_EQ(c->toString(), "[[\"0\",\"2\",\"4\"],[\"4\",\"0\",\"2\"]]");
}

TEST(Matrix, Mul)
{
  const uint32_t m = 2;
  const uint32_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  auto b = Integer::createWithString("2");
  for (uint32_t i = 0; i < m; i++)
  {
    for (uint32_t j = 0; j < n; j++)
    {
      a->values[i][j] = Integer::createWithString(to_string(i * n + j));
    }
  }

  auto c = a->mul(b);
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "2");
  EXPECT_EQ(c->toString(), "[[\"0\",\"2\",\"4\"],[\"6\",\"8\",\"10\"]]");
}

TEST(Matrix, ModMul)
{
  const uint32_t m = 2;
  const uint32_t n = 3;
  const auto modulus = Integer::createWithString("6");
  auto a = make_shared<Matrix>(m, n);
  auto b = Integer::createWithString("2");
  for (uint32_t i = 0; i < m; i++)
  {
    for (uint32_t j = 0; j < n; j++)
    {
      a->values[i][j] = Integer::createWithString(to_string(i * n + j));
    }
  }

  auto c = a->mul(b, modulus);
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "2");
  EXPECT_EQ(c->toString(), "[[\"0\",\"2\",\"4\"],[\"0\",\"2\",\"4\"]]");
}

TEST(Matrix, Dot)
{
  const uint32_t m = 2;
  const uint32_t n = 3;
  const uint32_t p = 2;
  auto a = make_shared<Matrix>(m, n);
  auto b = make_shared<Matrix>(n, p);
  for (uint32_t i = 0; i < m; i++)
  {
    for (uint32_t j = 0; j < n; j++)
    {
      a->values[i][j] = Integer::createWithString(to_string(i * n + j + 1));
      for (uint32_t k = 0; k < p; k++)
      {
        b->values[j][k] = Integer::create(to_string(m * n + j * p + k + 1), 10);
      }
    }
  }

  auto c = a->dot(b);
  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]");
  EXPECT_EQ(b->toString(), "[[\"7\",\"8\"],[\"9\",\"10\"],[\"11\",\"12\"]]");
  EXPECT_EQ(c->toString(), "[[\"58\",\"64\"],[\"139\",\"154\"]]");
}

TEST(Matrix, ModDot)
{
  const uint32_t m = 2;
  const uint32_t n = 3;
  const uint32_t p = 2;
  const auto modulus = Integer::createWithString("7");
  auto a = make_shared<Matrix>(m, n);
  auto b = make_shared<Matrix>(n, p);
  for (uint32_t i = 0; i < m; i++)
  {
    for (uint32_t j = 0; j < n; j++)
    {
      a->values[i][j] = Integer::createWithString(to_string(i * n + j + 1));
      for (uint32_t k = 0; k < p; k++)
      {
        b->values[j][k] = Integer::create(to_string(m * n + j * p + k + 1), 10);
      }
    }
  }

  auto c = a->dot(b, modulus);
  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]");
  EXPECT_EQ(b->toString(), "[[\"7\",\"8\"],[\"9\",\"10\"],[\"11\",\"12\"]]");
  EXPECT_EQ(c->toString(), "[[\"2\",\"1\"],[\"6\",\"0\"]]");
}

TEST(Matrix, AppendRow)
{
  const uint32_t m = 2;
  const uint32_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  for (uint32_t i = 0; i < m; i++)
  {
    for (uint32_t j = 0; j < n; j++)
    {
      a->values[i][j] = Integer::createWithString(to_string(i * n + j + 1));
    }
  }

  vector<shared_ptr<Integer>> b{
      Integer::createWithString("7"),
      Integer::createWithString("8"),
      Integer::createWithString("9")};

  EXPECT_EQ(a->m, m);
  EXPECT_EQ(a->n, n);
  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]");

  a->appendRow(b);
  EXPECT_EQ(a->m, m + 1);
  EXPECT_EQ(a->n, n);
  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"],[\"7\",\"8\",\"9\"]]");
}

TEST(Matrix, AppendCol)
{
  const uint32_t m = 2;
  const uint32_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  for (uint32_t i = 0; i < m; i++)
  {
    for (uint32_t j = 0; j < n; j++)
    {
      a->values[i][j] = Integer::createWithString(to_string(i * n + j + 1));
    }
  }

  vector<shared_ptr<Integer>> b{
      Integer::createWithString("7"),
      Integer::createWithString("8")};

  EXPECT_EQ(a->m, m);
  EXPECT_EQ(a->n, n);
  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]");

  a->appendCol(b);
  EXPECT_EQ(a->m, m);
  EXPECT_EQ(a->n, n + 1);
  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\",\"7\"],[\"4\",\"5\",\"6\",\"8\"]]");
}

TEST(Matrix, Power_vector)
{
  const uint32_t n = 6;
  auto x = Integer::TWO();
  auto a = Matrix::powerVector(x, n);
  auto b = a->t();

  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"4\",\"8\",\"16\",\"32\"]]");
  EXPECT_EQ(b->toString(), "[[\"1\"],[\"2\"],[\"4\"],[\"8\"],[\"16\"],[\"32\"]]");
}

TEST(Matrix, Power_vector_mod)
{
  const auto modulus = Integer::createWithString("7");
  const uint32_t n = 6;
  auto x = Integer::TWO();
  auto a = Matrix::powerVector(x, n, modulus);
  auto b = a->t();

  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"4\",\"1\",\"2\",\"4\"]]");
  EXPECT_EQ(b->toString(), "[[\"1\"],[\"2\"],[\"4\"],[\"1\"],[\"2\"],[\"4\"]]");
}

} // namespace
