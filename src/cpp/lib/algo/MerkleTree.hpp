#pragma once

#include "../namespace.hpp"

namespace cryptoplus
{

class MerkleTree
{
private:
  vector<binary_t> values;
  vector<vector<binary_t>> tree;
  binary_t root;
  unsigned int level, size;

  // Get Neighbour Index
  int getNeighbour(int index);

  /**
   * Compute Hash Size & Level
   */
  void precompute();

  /**
   * Get Hashed Result
   */
  vector<binary_t> getHashedParents(const vector<binary_t> &children);

public:
  // Validate Proofs
  static bool validate(int index, binary_t value, vector<binary_t> proofs, binary_t rootHash);

  MerkleTree(){};

  /**
   * Initialize the Tree with Hashes
   */
  MerkleTree(vector<binary_t> values);

  /**
   * Get The Data Size of the tree
   */
  size_t getSize();

  /**
   * Add a Hash into Tree
   */
  void add(binary_t data, bool compile = true);

  /**
   * Remove a Hash from Tree
   */
  void remove(int index, bool compile = true);

  /**
   * Compile the Tree
   */
  void compile();

  // Get Root Hash
  binary_t getRootHash();

  // Get All Necessary Proof with a value
  vector<binary_t> getProofs(int index);
};

} // namespace cryptoplus
