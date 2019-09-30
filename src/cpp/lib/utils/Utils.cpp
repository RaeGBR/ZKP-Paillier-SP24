#include "./UtilsImpl.hpp"

vector<uint8_t> Utils::calcHash(const vector<uint8_t> &message)
{
  return HashUtils::calcHash(message);
}

string Utils::binaryToString(const vector<uint8_t> &bytes)
{
  return HexUtils::binaryToString(bytes);
}

string Utils::binaryToHex(const vector<uint8_t> &bytes)
{
  return HexUtils::binaryToHex(bytes);
}

binary_t Utils::binaryConcat(const binary_t &a, const binary_t &b)
{
  return HexUtils::concat(a, b);
}

binary_t Utils::binaryPadZero(const binary_t &bytes, int length)
{
  return HexUtils::padZero(bytes, length);
}

vector<uint8_t> Utils::stringToBinary(const string &plainText)
{
  return HexUtils::stringToBinary(plainText);
}

string Utils::stringToHex(const string &plainText)
{
  return HexUtils::encode(plainText);
}

string Utils::hexToString(const string &plainText)
{
  return HexUtils::decode(plainText);
}

vector<uint8_t> Utils::hexToBinary(const string &hexString)
{
  string plainText = HexUtils::decode(hexString);
  return HexUtils::stringToBinary(plainText);
}

vector<uint8_t> Utils::random(int32_t byteLength)
{
  return RandomUtils::genBinary(byteLength);
}

vector<uint8_t> Utils::randomWithSeed(int32_t byteLength, const vector<uint8_t> &seed)
{
  return RandomUtils::genBinary(byteLength, seed);
}

string Utils::randomHex(int32_t byteLength)
{
  return RandomUtils::genHex(byteLength);
}

string Utils::randomHexWithSeed(int32_t byteLength, const vector<uint8_t> &seed)
{
  return RandomUtils::genHex(byteLength, seed);
}

shared_ptr<Integer> Utils::randomInt(int32_t byteLength)
{
  return RandomUtils::genInteger(byteLength, false);
}

shared_ptr<Integer> Utils::randomIntWithSeed(int32_t byteLength, const vector<uint8_t> &seed)
{
  return RandomUtils::genInteger(byteLength, false, seed);
}

vector<uint8_t> Utils::calcSignature(const shared_ptr<EC> &curve, const vector<uint8_t> &privateKey, const vector<uint8_t> &message)
{
  return ECSignature::sign(curve, privateKey, message);
}

bool Utils::verifySignature(const shared_ptr<EC> &curve, const vector<uint8_t> &publicKey, const vector<uint8_t> &message, const vector<uint8_t> &signature)
{
  return ECSignature::verify(curve, publicKey, message, signature);
}

vector<uint8_t> Utils::ecEncrypt(const shared_ptr<EC> &curve, const vector<uint8_t> &publicKey, const vector<uint8_t> &plaintext)
{
  return ECEncryption::encrypt(curve, publicKey, plaintext);
}

vector<uint8_t> Utils::ecDecrypt(const shared_ptr<EC> &curve, const vector<uint8_t> &privateKey, const vector<uint8_t> &cyphertext)
{
  return ECEncryption::decrypt(curve, privateKey, cyphertext);
}

vector<uint8_t> Utils::aesEncrypt(const vector<uint8_t> &key, const vector<uint8_t> &iv, const vector<uint8_t> &plainText)
{
  string ret = AESEncryption::aesEncrypt(HexUtils::binaryToString(key), HexUtils::binaryToString(iv), HexUtils::binaryToString(plainText));
  return HexUtils::stringToBinary(ret);
}

vector<uint8_t> Utils::aesDecrypt(const vector<uint8_t> &key, const vector<uint8_t> &iv, const vector<uint8_t> &encryptedText)
{
  string ret = AESEncryption::aesDecrypt(HexUtils::binaryToString(key), HexUtils::binaryToString(iv), HexUtils::binaryToString(encryptedText));
  return HexUtils::stringToBinary(ret);
}
