#include "./Matrix.hpp"

shared_ptr<Matrix> Matrix::ZERO()
{
  return make_shared<Matrix>();
}

shared_ptr<Matrix> Matrix::identity(size_t size)
{
  auto I = make_shared<Matrix>(size, size);
  for (size_t i = 0; i < size; i++)
  {
    I->values[i][i] = Integer::ONE();
  }
  return I;
}

shared_ptr<Matrix> Matrix::powerVector(const shared_ptr<Integer> &x,
                                       size_t n,
                                       const shared_ptr<Integer> &modulus)
{
  bool isMod = !Integer::ZERO()->eq(modulus);

  auto ret = make_shared<Matrix>(1, n);
  ret->values[0][0] = Integer::ONE();
  for (size_t i = 1; i < n; i++)
  {
    if (isMod)
    {
      ret->values[0][i] = ret->values[0][i - 1]->modMul(x, modulus);
    }
    else
    {
      ret->values[0][i] = ret->values[0][i - 1]->mul(x);
    }
  }
  return ret;
}

Matrix::Matrix() : Matrix(1, 1) {}

Matrix::Matrix(size_t m, size_t n)
{
  if (m <= 0 || n <= 0)
    throw invalid_argument("m and n cannot be zero");

  this->m = m;
  this->n = n;
  for (size_t i = 0; i < m; i++)
  {
    values.push_back(vector<shared_ptr<Integer>>(n));
    for (size_t j = 0; j < n; j++)
    {
      values[i][j] = Integer::ZERO();
    }
  }

  for (size_t i = 0; i < n; i++)
  {
    zeroVector.push_back(Integer::ZERO());
  }
}

Matrix::Matrix(const vector<int> &values)
{
  m = 1;
  if (m <= 0)
    throw invalid_argument("m and n cannot be zero");
  n = values.size();
  if (n <= 0)
    throw invalid_argument("m and n cannot be zero");

  this->values.push_back(vector<shared_ptr<Integer>>());
  for (size_t i = 0; i < n; i++)
  {
    this->values[0].push_back(make_shared<IntegerImpl>(values[i]));
    zeroVector.push_back(Integer::ZERO());
  }
}

Matrix::Matrix(const vector<shared_ptr<Integer>> &values) : Matrix(vector<vector<shared_ptr<Integer>>>{values})
{
}

Matrix::Matrix(const vector<vector<shared_ptr<Integer>>> &values)
{
  m = values.size();
  if (m <= 0)
    throw invalid_argument("m and n cannot be zero");
  n = values[0].size();
  if (n <= 0)
    throw invalid_argument("m and n cannot be zero");

  for (size_t i = 0; i < m; i++)
  {
    if (values[i].size() != n)
      throw invalid_argument("matrix initial values does not fit the matrix size");
    this->values.push_back(vector<shared_ptr<Integer>>(values[i].begin(), values[i].end()));
  }

  for (size_t i = 0; i < n; i++)
  {
    zeroVector.push_back(Integer::ZERO());
  }
}

shared_ptr<Matrix> Matrix::t()
{
  auto b = make_shared<Matrix>(n, m);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      b->values[j][i] = this->values[i][j];
    }
  }

  return b;
}

shared_ptr<Matrix> Matrix::add(const shared_ptr<Matrix> &b, const shared_ptr<Integer> &modulus)
{
  if (m != b->m || n != b->n)
    throw invalid_argument("matrix dimension not match for addition");

  bool isMod = !Integer::ZERO()->eq(modulus);

  auto ret = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      ret->values[i][j] = this->values[i][j]->add(b->values[i][j]);
      if (isMod)
      {
        ret->values[i][j] = ret->values[i][j]->mod(modulus);
      }
    }
  }
  return ret;
}

shared_ptr<Matrix> Matrix::mul(const shared_ptr<Integer> &b, const shared_ptr<Integer> &modulus)
{
  bool isMod = !Integer::ZERO()->eq(modulus);

  auto ret = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      if (isMod)
      {
        ret->values[i][j] = this->values[i][j]->modMul(b, modulus);
      }
      else
      {
        ret->values[i][j] = this->values[i][j]->mul(b);
      }
    }
  }
  return ret;
}

shared_ptr<Matrix> Matrix::mul(const shared_ptr<Matrix> &b, const shared_ptr<Integer> &modulus)
{
  if (n != b->m)
    throw invalid_argument("matrix dimension not match for cross product");

  auto p = b->n;
  bool isMod = !Integer::ZERO()->eq(modulus);

  auto ret = make_shared<Matrix>(m, p);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < p; j++)
    {
      for (size_t r = 0; r < n; r++)
      {
        if (isMod)
        {
          auto v = this->values[i][r]->modMul(b->values[r][j], modulus);
          ret->values[i][j] = ret->values[i][j]->add(v);
          ret->values[i][j] = ret->values[i][j]->mod(modulus);
        }
        else
        {
          auto v = this->values[i][r]->mul(b->values[r][j]);
          ret->values[i][j] = ret->values[i][j]->add(v);
        }
      }
    }
  }
  return ret;
}

shared_ptr<Matrix> Matrix::inner(const shared_ptr<Matrix> &b, const shared_ptr<Integer> &modulus)
{
  if (m != b->m || n != b->n)
    throw invalid_argument("matrix dimension not match for inner product");

  bool isMod = !Integer::ZERO()->eq(modulus);

  auto ret = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      if (isMod)
      {
        auto v = this->values[i][j]->modMul(b->values[i][j], modulus);
        ret->values[i][j] = v;
      }
      else
      {
        auto v = this->values[i][j]->mul(b->values[i][j]);
        ret->values[i][j] = v;
      }
    }
  }
  return ret;
}

shared_ptr<Matrix> Matrix::dot(const shared_ptr<Matrix> &b, const shared_ptr<Integer> &modulus)
{
  if (m != b->m || n != b->n)
    throw invalid_argument("matrix dimension not match for cross product");

  auto t = b->t();
  return this->mul(t, modulus);
}

vector<shared_ptr<Integer>> &Matrix::row(size_t i)
{
  if (i < 0 || i >= m)
    return zeroVector;

  return values[i];
}

void Matrix::appendRow(const vector<shared_ptr<Integer>> &row)
{
  if (row.size() != n)
    throw invalid_argument("matrix dimension not match for append row");

  m++;
  for (size_t i = 0; i < n; i++)
  {
    values.push_back(vector<shared_ptr<Integer>>(row.begin(), row.end()));
  }
}

void Matrix::appendCol(const vector<shared_ptr<Integer>> &col)
{
  if (col.size() != m)
    throw invalid_argument("matrix dimension not match for append row");

  n++;
  zeroVector.push_back(Integer::ZERO());
  for (size_t i = 0; i < m; i++)
  {
    values[i].push_back(col[i]);
  }
}

shared_ptr<Matrix> Matrix::clone()
{
  return make_shared<Matrix>(values);
}

shared_ptr<Matrix> Matrix::group(size_t newN, size_t newM)
{
  if (m != 1)
    throw invalid_argument("only allow group from vector to matrix");

  if (newM == 0)
  {
    newM = (n % newN) == 0 ? n / newN : (n / newN) + 1;
  }
  auto ret = make_shared<Matrix>(newM, newN);

  if (n > newN * newM)
    throw invalid_argument("cannot group a big vector to a small matrix");

  for (size_t i = 0; i < n; i++)
  {
    size_t x = i / newN;
    size_t y = i % newN;
    (*ret)[x][y] = this->values[0][i];
  }

  return ret;
}

bool Matrix::eq(const shared_ptr<Matrix> &b)
{
  if (this->m != b->m || this->n != b->n || this->values.size() != b->values.size())
    return false;

  for (size_t i = 0; i < this->values.size(); i++)
  {
    if (this->values[i].size() != b->values[i].size())
      return false;

    for (size_t j = 0; j < this->values[i].size(); j++)
    {
      if (!this->values[i][j]->eq(b->values[i][j]))
        return false;
    }
  }

  return true;
}

json Matrix::toJson()
{
  json output = json::array();
  for (size_t i = 0; i < m; i++)
  {
    output[i] = json::array();
    for (size_t j = 0; j < n; j++)
    {
      output[i][j] = values[i][j]->toString();
    }
  }
  return output;
}

string Matrix::toString()
{
  return this->toJson().dump();
}
