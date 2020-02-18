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
    I->cell(i, i, Integer::ONE());
  }
  return I;
}

shared_ptr<Matrix> Matrix::powerVector(const shared_ptr<Integer> &x,
                                       size_t n,
                                       const shared_ptr<Integer> &modulus)
{
  bool isMod = !Integer::ZERO()->eq(modulus);

  auto ret = make_shared<Matrix>(1, n);
  ret->cell(0, 0, Integer::ONE());
  for (size_t i = 1; i < n; i++)
  {
    if (isMod)
    {
      ret->cell(0, i, ret->cell(0, i - 1)->modMul(x, modulus));
    }
    else
    {
      ret->cell(0, i, ret->cell(0, i - 1)->mul(x));
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
    values.push_back(map<size_t, shared_ptr<Integer>>());
  }

  for (size_t i = 0; i < n; i++)
  {
    zeroVector.push_back(Integer::ZERO());
  }
}

Matrix::Matrix(const vector<int> &values) : Matrix::Matrix(1, values.size())
{
  for (size_t i = 0; i < n; i++)
  {
    this->cell(0, i, make_shared<IntegerImpl>(values[i]));
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

    this->values.push_back(map<size_t, shared_ptr<Integer>>());
    for (size_t j = 0; j < n; j++)
    {
      this->cell(i, j, values[i][j]);
    }
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
    for (auto it : values[i])
    {
      size_t j = it.first;
      auto v = it.second;
      b->cell(j, i, v);
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
    for (auto it : values[i])
    {
      size_t j = it.first;
      auto v = it.second;
      ret->cell(i, j, v);
    }
    for (auto it : b->values[i])
    {
      size_t j = it.first;
      auto v1 = it.second;
      auto v2 = ret->cell(i, j);
      auto v = v1->add(v2);
      v = isMod ? v->mod(modulus) : v;
      ret->cell(i, j, v);
    }
  }
  return ret;
}

shared_ptr<Matrix> Matrix::mul(const shared_ptr<Integer> &b, const shared_ptr<Integer> &modulus)
{
  if (b->eq(Integer::ZERO()))
    return make_shared<Matrix>(m, n);

  if (b->eq(Integer::ONE()))
    return this->clone();

  bool isMod = !Integer::ZERO()->eq(modulus);

  auto ret = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (auto it : values[i])
    {
      size_t j = it.first;
      auto v = it.second;
      v = isMod ? v->modMul(b, modulus) : v->mul(b);
      ret->cell(i, j, v);
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

  // TODO: improve performance?
  auto ret = make_shared<Matrix>(m, p);
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < p; j++)
    {
      for (size_t r = 0; r < n; r++)
      {
        if (isMod)
        {
          auto v = this->cell(i, r)->modMul(b->cell(r, j), modulus);
          ret->cell(i, j, ret->cell(i, j)->add(v));
          ret->cell(i, j, ret->cell(i, j)->mod(modulus));
        }
        else
        {
          auto v = this->cell(i, r)->mul(b->cell(r, j));
          ret->cell(i, j, ret->cell(i, j)->add(v));
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
    for (auto it : values[i])
    {
      size_t j = it.first;
      auto v1 = it.second;
      auto v2 = b->cell(i, j);
      auto v = isMod ? v1->modMul(v2, modulus) : v1->mul(v2);
      ret->cell(i, j, v);
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

bool Matrix::rowExists(size_t i)
{
  return i < m && values[i].size() > 0;
}

bool Matrix::cellExists(size_t i, size_t j)
{
  return rowExists(i) && j < n && values[i].find(j) != values[i].end();
}

shared_ptr<Matrix> Matrix::rowAsMatrix(size_t i)
{
  auto ret = make_shared<Matrix>(1, n);

  if (rowExists(i))
  {
    for (auto it : values[i])
    {
      size_t j = it.first;
      auto v = it.second;
      ret->cell(0, j, v);
    }
  }

  return ret;
}

vector<shared_ptr<Integer>> Matrix::row(size_t i)
{
  if (!rowExists(i))
    return zeroVector;

  vector<shared_ptr<Integer>> ret;
  for (size_t j = 0; j < n; j++)
  {
    ret.push_back(cell(i, j));
  }
  return ret;
}

shared_ptr<Integer> Matrix::cell(size_t i, size_t j)
{
  return cellExists(i, j) ? values[i][j] : Integer::ZERO();
}

void Matrix::cell(size_t i, size_t j, const shared_ptr<Integer> &x)
{
  if (i >= m || j >= n)
    throw invalid_argument("index out of the matrix dimension");

  if (x->eq(Integer::ZERO()))
  {
    values[i].erase(j);
  }
  else
  {
    values[i][j] = x;
  }
}

void Matrix::shift(size_t n)
{
  if (n == 0)
    return;

  this->n += n;

  for (size_t i = 0; i < n; i++)
  {
    zeroVector.push_back(Integer::ZERO());
  }

  for (size_t i = 0; i < m; i++)
  {
    vector<size_t> keys;
    vector<shared_ptr<Integer>> cells;
    for (auto it : this->values[i])
    {
      keys.push_back(it.first);
      cells.push_back(it.second);
    }
    this->values[i].clear();
    for (size_t j = 0; j < keys.size(); j++)
    {
      auto key = n + keys[j];
      this->cell(i, key, cells[j]);
    }
  }
}

void Matrix::extend(size_t n)
{
  this->n += n;

  for (size_t i = 0; i < n; i++)
  {
    zeroVector.push_back(Integer::ZERO());
  }
}

void Matrix::trim()
{
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      if (cellExists(i, j))
      {
        if (values[i][j]->eq(Integer::ZERO()))
        {
          values[i].erase(j);
        }
      }
    }
  }
}

void Matrix::appendRow(const vector<shared_ptr<Integer>> &row)
{
  if (row.size() != n)
    throw invalid_argument("matrix dimension not match for append row");

  m++;
  values.push_back(map<size_t, shared_ptr<Integer>>());
  for (size_t i = 0; i < n; i++)
  {
    this->cell(m - 1, i, row[i]);
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
    this->cell(i, n - 1, col[i]);
  }
}

shared_ptr<Matrix> Matrix::clone()
{
  auto ret = make_shared<Matrix>(m, n);
  for (size_t i = 0; i < m; i++)
  {
    for (auto it : values[i])
    {
      size_t j = it.first;
      auto v = it.second;
      ret->cell(i, j, v);
    }
  }

  return ret;
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

  for (auto it : values[0])
  {
    size_t i = it.first;
    auto v = it.second;
    size_t x = i / newN;
    size_t y = i % newN;
    ret->cell(x, y, v);
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
      if (!this->cell(i, j)->eq(b->cell(i, j)))
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
      output[i][j] = cell(i, j)->toString();
    }
  }
  return output;
}

string Matrix::toString()
{
  return this->toJson().dump();
}
