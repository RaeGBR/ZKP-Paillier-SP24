#include "MerkleTree.hpp"

/*
 * Static method
 */
// Validate Proofs
bool MerkleTree::validate(int index, binary_t value, vector<binary_t> proofs, binary_t rootHash)
{
  // index out of bound of a possible tree size
  int max = pow(2, proofs.size());
  if (index < 0 || index >= max)
    return false;

  int currentIndex = index;
  binary_t currentHash = HashUtils::calcHash(value);

  for (int i = 0; i < proofs.size(); i++)
  {
    binary_t neighbour = proofs[i];
    bool isLeft = currentIndex % 2 == 0;
    // calculate a propagate up
    binary_t combined = isLeft ? HexUtils::concat(currentHash, neighbour) : HexUtils::concat(neighbour, currentHash);
    currentHash = HashUtils::calcHash(combined);
    currentIndex = currentIndex / 2;
  }
  return HexUtils::eq(currentHash, rootHash);
}

/*
 * Constructor
 */
/**
 * Initialize the Tree with Hashes
 */
MerkleTree::MerkleTree(vector<binary_t> values)
{
  for (auto value : values)
    this->add(value, false);
  this->compile();
}

/*
 * Private method
 */
// Get Neighbour Index
int MerkleTree::getNeighbour(int index)
{
  return index % 2 == 0 ? index + 1 : index - 1;
}

/**
 * Compute Hash Size & Level
 */
void MerkleTree::precompute()
{
  size = 0;
  level = 0;
  for (int i = getSize(); i > 1; i = (i + 1) >> 1)
  {
    size += i;
    level++;
  }
  level++;
  size = size >= 2 ? ++size : 1;

  // Create Empty Vector for Coming Hashes
  tree.resize(1);
}

/**
 * Get Hashed Result
 */
vector<binary_t> MerkleTree::getHashedParents(const vector<binary_t> &children)
{
  vector<binary_t> result;

  for (int i = 0; i < children.size();)
  {
    binary_t a = children[i]; // , b = children[i];
    // Method 1 : Single Leaf -> Hash itself
    binary_t b = ++i < children.size() ? children[i++] : binary_t();
    // Method 2 : Single Leaf -> Hash (itself + itself)
    // if (++i < children.size()) b = children[i++];
    binary_t data = HexUtils::concat(a, b); // HashUtils::Hash(a, b)
    binary_t hash = HashUtils::calcHash(data);
    // printf("hash(%s, %s)=>%s \n", a.c_str(), b.c_str(), hash.c_str());
    result.push_back(hash);
  }
  // printf("\n");
  return result;
}

/*
 * Public method
 */
/**
 * Get The Data Size of the tree
 */
size_t MerkleTree::getSize() { return values.size(); }

/**
 * Add a Hash into Tree
 */
void MerkleTree::add(binary_t data, bool compile)
{
  values.push_back(data);
  precompute();
  // RESIZE THE TREE with Pre-calculated Tree Level!!!!!!!!!!!!!
  this->tree[0].push_back(HashUtils::calcHash(data));
  if (compile)
    this->compile();
}

/**
 * Remove a Hash from Tree
 */
void MerkleTree::remove(int index, bool compile)
{
  if (index < 0)
  {
    throw std::invalid_argument("index must be >= 0");
  }
  if (values.empty())
  {
    throw std::logic_error("Tree is empty");
  }
  if (values.size() < index)
  {
    throw std::out_of_range("index is out of range");
  }
  values.erase(values.begin() + index);
  this->tree[0].erase(this->tree[0].begin() + index);
  if (compile)
    this->compile();
}

/**
 * Compile the Tree
 */
void MerkleTree::compile()
{
  vector<binary_t> current;

  // Single Leaf
  if (size == 1)
  {
    this->root = this->tree[0][0];
    return;
  }

  current = this->tree[0];
  while (current.size() != 1)
  {
    current = getHashedParents(current);
    tree.push_back(current);
  }
  this->root = current[0];
}

// Get Root Hash
binary_t MerkleTree::getRootHash() { return root; }

// Get All Necessary Proof with a value
vector<binary_t> MerkleTree::getProofs(int index)
{
  vector<binary_t> proofs;

  if (size > 1)
  {
    // Get Each Level's Neighbour except the Root Hash
    for (int i = 0; i < (level - 1); i++)
    {
      // Get Level's Leaf Size
      size_t levelSize = this->tree[i].size();
      int near = getNeighbour(index);
      // Get Necessary Neighbour Hash
      binary_t neighbour = near >= 0 && near < levelSize ? this->tree[i][near] : binary_t();
      proofs.push_back(neighbour);
      index = index / 2;
    }
  }
  return proofs;
}
