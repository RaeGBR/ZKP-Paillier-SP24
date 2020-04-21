#include "gtest/gtest.h"

#include "app/math/Matrix.hpp"

namespace
{

TEST(Matrix, Default)
{
  ZZ_p::init(conv<ZZ>(1000000));
  const size_t m = 2;
  const size_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  EXPECT_EQ(a->m, m);
  EXPECT_EQ(a->n, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      EXPECT_EQ(a->cell(i, j), 0);
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
      EXPECT_EQ(a->cell(i, j), i * n + j);
    }
  }
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
      a->cell(i, j, i * n + j);
    }
  }

  auto b = make_shared<Matrix>((*a));
  auto c = b->clone();
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(c->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");

  a->cell(0, 0, 7);
  b->cell(0, 0, 8);
  c->cell(0, 0, 9);
  EXPECT_EQ(a->toString(), "[[\"7\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"8\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(c->toString(), "[[\"9\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");

  c->cell(0, 0, 1);
  EXPECT_EQ(a->toString(), "[[\"7\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(b->toString(), "[[\"8\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
  EXPECT_EQ(c->toString(), "[[\"1\",\"1\",\"2\"],[\"3\",\"4\",\"5\"]]");
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
      a->cell(i, j, i * n + j);
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
      a->cell(i, j, i * n + j);
    }
  }

  auto b = a->group(3, 3);
  auto c = a->group(4, 2);
  EXPECT_EQ(a->toString(), "[[\"0\",\"1\",\"2\",\"3\",\"4\"]]");
  EXPECT_EQ(b->toString(), "[[\"0\",\"1\",\"2\"],[\"3\",\"4\",\"0\"],[\"0\",\"0\",\"0\"]]");
  EXPECT_EQ(c->toString(), "[[\"0\",\"1\",\"2\",\"3\"],[\"4\",\"0\",\"0\",\"0\"]]");
}

TEST(Matrix, Shift)
{
  ZZ_p::init(conv<ZZ>(100));
  const size_t m = 2;
  const size_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      a->cell(i, j, conv<ZZ_p>(i * n + j + 1));
    }
  }

  EXPECT_EQ(a->m, m);
  EXPECT_EQ(a->n, n);
  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]");

  size_t offset = 3;
  a->shift(offset);
  EXPECT_EQ(a->m, m);
  EXPECT_EQ(a->n, n + offset);
  EXPECT_EQ(a->toString(), "[[\"0\",\"0\",\"0\",\"1\",\"2\",\"3\"],[\"0\",\"0\",\"0\",\"4\",\"5\",\"6\"]]");
}

TEST(Matrix, Extend)
{
  ZZ_p::init(conv<ZZ>(100));
  const size_t m = 2;
  const size_t n = 3;
  auto a = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      a->cell(i, j, conv<ZZ_p>(i * n + j + 1));
    }
  }

  EXPECT_EQ(a->m, m);
  EXPECT_EQ(a->n, n);
  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]");

  size_t offset = 3;
  a->extend(offset);
  EXPECT_EQ(a->m, m);
  EXPECT_EQ(a->n, n + offset);
  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\",\"0\",\"0\",\"0\"],[\"4\",\"5\",\"6\",\"0\",\"0\",\"0\"]]");
}

TEST(Matrix, Trim)
{
  auto a = make_shared<Matrix>(vector<int>({1, 2, 3, 0, 0, 0, 4, 5, 6}));

  EXPECT_EQ(a->m, 1);
  EXPECT_EQ(a->n, 9);
  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\",\"0\",\"0\",\"0\",\"4\",\"5\",\"6\"]]");

  a = a->group(3);
  EXPECT_EQ(a->m, 3);
  EXPECT_EQ(a->n, 3);
  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\"],[\"0\",\"0\",\"0\"],[\"4\",\"5\",\"6\"]]");

  a->trim();
  EXPECT_EQ(a->m, 3);
  EXPECT_EQ(a->n, 3);
  EXPECT_EQ(a->values[0].size(), 3);
  EXPECT_EQ(a->values[1].size(), 0);
  EXPECT_EQ(a->values[2].size(), 3);
  EXPECT_EQ(a->toString(), "[[\"1\",\"2\",\"3\"],[\"0\",\"0\",\"0\"],[\"4\",\"5\",\"6\"]]");
}

TEST(Matrix, Equal)
{
  auto a = make_shared<Matrix>(2, 2);
  auto b = make_shared<Matrix>(2, 2);

  EXPECT_EQ(a->eq(b), true);

  b->cell(0, 0, 1);
  b->cell(1, 1, 1);

  EXPECT_EQ(a->eq(b), false);

  auto c = make_shared<Matrix>(vector<int>({1, 0, 0, 1}))->group(2);
  EXPECT_EQ(b->eq(c), true);
}

} // namespace
