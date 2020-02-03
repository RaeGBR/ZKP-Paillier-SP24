#include "gtest/gtest.h"

#include "lib/math/Matrix.hpp"

using namespace std;
using namespace cryptoplus;

namespace
{

TEST(Matrix, Default)
{
  const size_t m = 2;
  const size_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  EXPECT_EQ(a->m, m);
  EXPECT_EQ(a->n, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      EXPECT_EQ((*a)[i][j]->toString(), "0");
    }
  }

  EXPECT_EQ(a->toString(), "[[\"0\",\"0\",\"0\"],[\"0\",\"0\",\"0\"]]");
}

TEST(Matrix, Constructor_int_array)
{
  int m = 1;
  int n = 6;
  auto a = make_shared<Matrix>(vector<int>({0, 1, 2, 3, 4, 5}));
  EXPECT_EQ(a->m, m);
  EXPECT_EQ(a->n, n);

  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      EXPECT_EQ((*a)[i][j]->toString(), to_string(i * n + j));
    }
  }
}

TEST(Matrix, Identity)
{
  const size_t size = 3;
  auto a = Matrix::identity(size);
  EXPECT_EQ(a->m, size);
  EXPECT_EQ(a->n, size);
  for (size_t i = 0; i < size; i++)
  {
    for (size_t j = 0; j < size; j++)
    {
      string expected = i == j ? "1" : "0";
      EXPECT_EQ((*a)[i][j]->toString(), expected);
    }
  }

  EXPECT_EQ(a->toString(), "[[\"1\",\"0\",\"0\"],[\"0\",\"1\",\"0\"],[\"0\",\"0\",\"1\"]]");
}

TEST(Matrix, Clone)
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

  auto b = make_shared<Matrix>((*a));
  auto c = b->clone();
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(c->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");

  (*c)[0][0] = Integer::createWithString("9");
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(c->toString(), "[[\"9\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
}

TEST(Matrix, Group)
{
  const size_t m = 1;
  const size_t n = 5;
  auto a = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*a)[i][j] = Integer::createWithString(to_string(i * n + j));
    }
  }

  auto b = a->group(3);
  auto c = a->group(2);
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\",\"3\",\"4\"]]");
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"0\"]]");
  EXPECT_EQ(c->toString(), "[[\"0\",\"1\"],[\"2\",\"3\"],[\"4\",\"0\"]]");
}

TEST(Matrix, Group_to_big_matrix)
{
  const size_t m = 1;
  const size_t n = 5;
  auto a = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*a)[i][j] = Integer::createWithString(to_string(i * n + j));
    }
  }

  auto b = a->group(3, 3);
  auto c = a->group(4, 2);
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\",\"3\",\"4\"]]");
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"0\"],[\"0\",\"0\",\"0\"]]");
  EXPECT_EQ(c->toString(), "[[\"0\",\"1\",\"2\",\"3\"],[\"4\",\"0\",\"0\",\"0\"]]");
}

TEST(Matrix, Tansposition)
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

  auto b = a->t();
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"0\",\"3\"],[\"1\",\"4\"],[\"2\",\"5\"]]");
}

TEST(Matrix, Add)
{
  const size_t m = 2;
  const size_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  auto b = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*a)[i][j] = Integer::createWithString(to_string(i * n + j));
      (*b)[i][j] = Integer::createWithString(to_string(i + j));
    }
  }

  auto c = a->add(b);
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"1\",\"2\",\"3\"]]");
  EXPECT_EQ(c->toString(), "[[\"0\",\"2\",\"4\"],[\"4\",\"6\",\"8\"]]");
}

TEST(Matrix, ModAdd)
{
  const size_t m = 2;
  const size_t n = 3;
  const auto modulus = Integer::createWithString("6");
  auto a = make_shared<Matrix>(m, n);
  auto b = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*a)[i][j] = Integer::createWithString(to_string(i * n + j));
      (*b)[i][j] = Integer::createWithString(to_string(i + j));
    }
  }

  auto c = a->add(b, modulus);
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"1\",\"2\",\"3\"]]");
  EXPECT_EQ(c->toString(), "[[\"0\",\"2\",\"4\"],[\"4\",\"0\",\"2\"]]");
}

TEST(Matrix, ModAdd_with_negative)
{
  const auto modulus = Integer::createWithString("7");
  auto a = make_shared<Matrix>();
  auto b = make_shared<Matrix>();
  (*a)[0][0] = Integer::createWithString(to_string(-3));
  (*b)[0][0] = Integer::createWithString(to_string(4));
  auto c = a->add(b, modulus);
  EXPECT_EQ(a->toString(), "[[\"-3\"]]");
  EXPECT_EQ(b->toString(), "[[\"4\"]]");
  EXPECT_EQ(c->toString(), "[[\"1\"]]");
}

TEST(Matrix, Mul)
{
  const size_t m = 2;
  const size_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  auto b = Integer::createWithString("2");
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*a)[i][j] = Integer::createWithString(to_string(i * n + j));
    }
  }

  auto c = a->mul(b);
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "2");
  EXPECT_EQ(c->toString(), "[[\"0\",\"2\",\"4\"],[\"6\",\"8\",\"10\"]]");
}

TEST(Matrix, ModMul)
{
  const size_t m = 2;
  const size_t n = 3;
  const auto modulus = Integer::createWithString("6");
  auto a = make_shared<Matrix>(m, n);
  auto b = Integer::createWithString("2");
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*a)[i][j] = Integer::createWithString(to_string(i * n + j));
    }
  }

  auto c = a->mul(b, modulus);
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "2");
  EXPECT_EQ(c->toString(), "[[\"0\",\"2\",\"4\"],[\"0\",\"2\",\"4\"]]");
}

TEST(Matrix, ModMul_with_negative)
{
  const auto modulus = Integer::createWithString("7");
  auto a = make_shared<Matrix>();
  auto b = make_shared<Matrix>();
  (*a)[0][0] = Integer::createWithString(to_string(-3));
  (*b)[0][0] = Integer::createWithString(to_string(4));
  auto c = a->mul(b, modulus);
  EXPECT_EQ(a->toString(), "[[\"-3\"]]");
  EXPECT_EQ(b->toString(), "[[\"4\"]]");
  EXPECT_EQ(c->toString(), "[[\"2\"]]");
}

TEST(Matrix, CrossProduct)
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

  auto c = a->mul(b);
  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]");
  EXPECT_EQ(b->toString(), "[[\"7\",\"8\"],[\"9\",\"10\"],[\"11\",\"12\"]]");
  EXPECT_EQ(c->toString(), "[[\"58\",\"64\"],[\"139\",\"154\"]]");
}

TEST(Matrix, ModCrossProduct)
{
  const size_t m = 2;
  const size_t n = 3;
  const size_t p = 2;
  const auto modulus = Integer::createWithString("7");
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

  auto c = a->mul(b, modulus);
  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]");
  EXPECT_EQ(b->toString(), "[[\"7\",\"8\"],[\"9\",\"10\"],[\"11\",\"12\"]]");
  EXPECT_EQ(c->toString(), "[[\"2\",\"1\"],[\"6\",\"0\"]]");
}

TEST(Matrix, Dot)
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

  auto c = a->dot(b);
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]");
  EXPECT_EQ(c->toString(), "[[\"8\",\"17\"],[\"26\",\"62\"]]");

  auto d = make_shared<Matrix>(1, n);
  auto e = make_shared<Matrix>(1, n);
  for (size_t i = 0; i < n; i++)
  {
    (*d)[0][i] = Integer::create(to_string(i + 1), 10);
    (*e)[0][i] = Integer::create(to_string(i + 1), 10);
  }

  auto f = d->dot(e);
  EXPECT_EQ(d->toString(), "[[\"1\",\"2\",\"3\"]]");
  EXPECT_EQ(e->toString(), "[[\"1\",\"2\",\"3\"]]");
  EXPECT_EQ(f->toString(), "[[\"14\"]]");
}

TEST(Matrix, ModDot)
{
  const size_t m = 2;
  const size_t n = 3;
  const auto modulus = Integer::createWithString("7");
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

  auto c = a->dot(b, modulus);
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]");
  EXPECT_EQ(c->toString(), "[[\"1\",\"3\"],[\"5\",\"6\"]]");

  auto d = make_shared<Matrix>(1, n);
  auto e = make_shared<Matrix>(1, n);
  for (size_t i = 0; i < n; i++)
  {
    (*d)[0][i] = Integer::create(to_string(i + 1), 10);
    (*e)[0][i] = Integer::create(to_string(i + 1), 10);
  }

  auto f = d->dot(e, modulus);
  EXPECT_EQ(d->toString(), "[[\"1\",\"2\",\"3\"]]");
  EXPECT_EQ(e->toString(), "[[\"1\",\"2\",\"3\"]]");
  EXPECT_EQ(f->toString(), "[[\"0\"]]");
}

TEST(Matrix, AppendRow)
{
  const size_t m = 2;
  const size_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*a)[i][j] = Integer::createWithString(to_string(i * n + j + 1));
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
  const size_t m = 2;
  const size_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*a)[i][j] = Integer::createWithString(to_string(i * n + j + 1));
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
  const size_t n = 6;
  auto x = Integer::TWO();
  auto a = Matrix::powerVector(x, n);
  auto b = a->t();

  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"4\",\"8\",\"16\",\"32\"]]");
  EXPECT_EQ(b->toString(), "[[\"1\"],[\"2\"],[\"4\"],[\"8\"],[\"16\"],[\"32\"]]");
}

TEST(Matrix, Power_vector_mod)
{
  const auto modulus = Integer::createWithString("7");
  const size_t n = 6;
  auto x = Integer::TWO();
  auto a = Matrix::powerVector(x, n, modulus);
  auto b = a->t();

  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"4\",\"1\",\"2\",\"4\"]]");
  EXPECT_EQ(b->toString(), "[[\"1\"],[\"2\"],[\"4\"],[\"1\"],[\"2\"],[\"4\"]]");
}

TEST(Matrix, Equal)
{
  auto a = make_shared<Matrix>(2, 2);
  auto b = make_shared<Matrix>(2, 2);

  EXPECT_EQ(a->eq(b), true);

  (*b)[0][0] = Integer::ONE();
  (*b)[1][1] = Integer::ONE();

  EXPECT_EQ(a->eq(b), false);

  auto c = Matrix::identity(2);
  EXPECT_EQ(b->eq(c), true);
}

} // namespace
