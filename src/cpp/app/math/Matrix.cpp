#include "./Matrix.hpp"

shared_ptr<Matrix> Matrix::ZERO()
{
  return make_shared<Matrix>();
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
    values.push_back(map<size_t, ZZ_p>());
  }
}

Matrix::Matrix(const vector<int> &values) : Matrix::Matrix(1, values.size())
{
  for (size_t i = 0; i < n; i++)
  {
    this->cell(0, i, values[i]);
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

bool Matrix::cellExists(size_t i, size_t j)
{
  return i < m && j < n && values[i].find(j) != values[i].end();
}

ZZ_p Matrix::cell(size_t i, size_t j)
{
  return cellExists(i, j) ? values[i][j] : ZZ_p();
}

void Matrix::cell(size_t i, size_t j, long x)
{
  cell(i, j, conv<ZZ_p>(x));
}

void Matrix::cell(size_t i, size_t j, const ZZ_p &x)
{
  if (i >= m || j >= n)
    throw invalid_argument("index out of the matrix dimension");

  if (IsZero(x))
  {
    values[i].erase(j);
  }
  else
  {
    values[i][j] = x;
  }
}

bool Matrix::rowExists(size_t i)
{
  return i < m && values[i].size() > 0;
}

void Matrix::toMat(Mat<ZZ_p> &output)
{
  output.SetDims(m, n);

  for (size_t i = 0; i < m; i++)
  {
    for (auto it : values[i])
    {
      size_t j = it.first;
      auto v = it.second;
      output[i][j] = v;
    }
  }
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

void Matrix::shift(size_t n)
{
  if (n == 0)
    return;

  this->n += n;

  for (size_t i = 0; i < m; i++)
  {
    vector<size_t> keys;
    vector<ZZ_p> cells;
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
}

void Matrix::trim()
{
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      if (cellExists(i, j))
      {
        if (IsZero(values[i][j]))
        {
          values[i].erase(j);
        }
      }
    }
  }
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
      if (this->cell(i, j) != b->cell(i, j))
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
      output[i][j] = ConvertUtils::toString(cell(i, j));
    }
  }
  return output;
}

string Matrix::toString()
{
  return this->toJson().dump();
}
