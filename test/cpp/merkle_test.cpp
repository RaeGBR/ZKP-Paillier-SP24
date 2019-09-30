#include "gtest/gtest.h"

#include "lib/namespace.hpp"
#include "lib/utils/HexUtils.hpp"
#include "lib/utils/HashUtils.hpp"
#include "lib/algo/MerkleTree.hpp"

namespace
{

binary_t v1 = Utils::hexToBinary("1");
binary_t v2 = Utils::hexToBinary("2");
binary_t v3 = Utils::hexToBinary("3");
binary_t h1 = HashUtils::calcHash(v1);
binary_t h2 = HashUtils::calcHash(v2);
binary_t h3 = HashUtils::calcHash(v3);
binary_t h12 = HashUtils::calcHash(HexUtils::concat(h1, h2));
binary_t h34 = HashUtils::calcHash(h3);
binary_t h1234 = HashUtils::calcHash(HexUtils::concat(h12, h34));

binary_t empty = Utils::stringToBinary("");

// Following the previous Java Test Cases
TEST(MerkleTree, Get_Root_Hash)
{
  MerkleTree m0 = MerkleTree();
  EXPECT_EQ(m0.getRootHash(), empty);

  MerkleTree m1 = MerkleTree();
  m1.add(v1);
  EXPECT_EQ(m1.getRootHash(), h1);

  MerkleTree m12 = MerkleTree({v1, v2});
  EXPECT_EQ(m12.getRootHash(), h12);

  MerkleTree m123 = MerkleTree();
  m123.add(v1, false);
  m123.add(v2, false);
  m123.add(v3, false);
  m123.compile();
  EXPECT_EQ(m123.getRootHash(), h1234);
}

TEST(MerkleTree, GetProofs)
{
  MerkleTree m = MerkleTree({v1, v2, v3});
  vector<binary_t> ex1 = {h2, h34};
  vector<binary_t> ans1 = m.getProofs(0);
  EXPECT_EQ(ex1.size(), ans1.size());
  EXPECT_EQ(ex1[0], ans1[0]);
  EXPECT_EQ(ex1[1], ans1[1]);

  vector<binary_t> ex2 = {h1, h34};
  vector<binary_t> ans2 = m.getProofs(1);
  EXPECT_EQ(ex2.size(), ans2.size());
  EXPECT_EQ(ex2[0], ans2[0]);
  EXPECT_EQ(ex2[1], ans2[1]);

  vector<binary_t> ex3 = {empty, h12};
  vector<binary_t> ans3 = m.getProofs(2);
  EXPECT_EQ(ex3.size(), ans3.size());
  EXPECT_EQ(ex3[0], ans3[0]);
  EXPECT_EQ(ex3[1], ans3[1]);

  vector<binary_t> ex4 = MerkleTree({v1}).getProofs(0);
  EXPECT_EQ(ex4.size(), 0);
}

TEST(MerkleTree, Proofs_Validation)
{
  EXPECT_TRUE(MerkleTree::validate(0, v1, {h2, h34}, h1234));
  EXPECT_FALSE(MerkleTree::validate(0, v1, {h34}, h1234));         // not enough proofs
  EXPECT_FALSE(MerkleTree::validate(0, v1, {h1, h2, h34}, h1234)); // too much proofs
  EXPECT_FALSE(MerkleTree::validate(0, v1, {h2, h34}, h1));        // not correct root hash
  EXPECT_FALSE(MerkleTree::validate(0, v2, {h2, h34}, h1));        // not correct value
  EXPECT_FALSE(MerkleTree::validate(1, v1, {h2, h34}, h1));        // not correct index
  EXPECT_FALSE(MerkleTree::validate(4, v1, {h2, h34}, h1));        // index excced the tree size

  EXPECT_TRUE(MerkleTree::validate(1, v2, {h1, h34}, h1234));
  EXPECT_TRUE(MerkleTree::validate(2, v3, {empty, h12}, h1234));
  EXPECT_TRUE(MerkleTree::validate(3, v3, {empty, h12}, h1234)); // odd number of values, last value index may swapable
}

TEST(MerkleTree, Tree_Proofs)
{
  MerkleTree m1 = MerkleTree({Utils::stringToBinary("0"),
                              Utils::stringToBinary("1"),
                              Utils::stringToBinary("2"),
                              Utils::stringToBinary("3"),
                              Utils::stringToBinary("4"),
                              Utils::stringToBinary("5"),
                              Utils::stringToBinary("6"),
                              Utils::stringToBinary("7"),
                              Utils::stringToBinary("8"),
                              Utils::stringToBinary("9")});

  EXPECT_TRUE(MerkleTree::validate(0, Utils::stringToBinary("0"), m1.getProofs(0), m1.getRootHash()));
  EXPECT_TRUE(MerkleTree::validate(5, Utils::stringToBinary("5"), m1.getProofs(5), m1.getRootHash()));
  EXPECT_TRUE(MerkleTree::validate(9, Utils::stringToBinary("9"), m1.getProofs(9), m1.getRootHash()));
  EXPECT_FALSE(MerkleTree::validate(0, Utils::stringToBinary("1"), m1.getProofs(0), m1.getRootHash()));

  MerkleTree m2 = MerkleTree({empty});
  EXPECT_TRUE(MerkleTree::validate(0, empty, m2.getProofs(0), m2.getRootHash()));
  // There is no NULL handling in string
  // EXPECT_TRUE(MerkleTree::validate(0, NULL, m2.getProofs(0), m2.getRootHash()));
  // EXPECT_TRUE(MerkleTree::validate(1, empty, m2.getProofs(1), m2.getRootHash()));
}

TEST(MerkleTree, remove)
{
  MerkleTree m1 = MerkleTree({Utils::stringToBinary("foo")});

  EXPECT_THROW(m1.remove(3), std::out_of_range);
  EXPECT_THROW(m1.remove(-1), std::invalid_argument);

  m1.remove(0);
  EXPECT_THROW(m1.remove(1), std::logic_error);
}

} // namespace
