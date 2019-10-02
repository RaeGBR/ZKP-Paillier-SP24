#include "CryptoUtils.hpp"

#include "../namespace.hpp"

#include "../basic/AESEncryption.hpp"

#include "../ec/ECEncryption.hpp"
#include "../ec/ECSignature.hpp"

#include "./HexUtils.hpp"

vector<uint8_t> CryptoUtils::ecGenerateSignature(const shared_ptr<ECCurve> &curve, const vector<uint8_t> &privateKey, const vector<uint8_t> &message)
{
  return ECSignature::sign(curve, privateKey, message);
}

bool CryptoUtils::ecVerifySignature(const shared_ptr<ECCurve> &curve, const vector<uint8_t> &publicKey, const vector<uint8_t> &message, const vector<uint8_t> &signature)
{
  return ECSignature::verify(curve, publicKey, message, signature);
}

vector<uint8_t> CryptoUtils::ecEncrypt(const shared_ptr<ECCurve> &curve, const vector<uint8_t> &publicKey, const vector<uint8_t> &plaintext)
{
  return ECEncryption::encrypt(curve, publicKey, plaintext);
}

vector<uint8_t> CryptoUtils::ecDecrypt(const shared_ptr<ECCurve> &curve, const vector<uint8_t> &privateKey, const vector<uint8_t> &cyphertext)
{
  return ECEncryption::decrypt(curve, privateKey, cyphertext);
}

vector<uint8_t> CryptoUtils::aesEncrypt(const vector<uint8_t> &key, const vector<uint8_t> &iv, const vector<uint8_t> &plainText)
{
  string ret = AESEncryption::aesEncrypt(HexUtils::binaryToString(key), HexUtils::binaryToString(iv), HexUtils::binaryToString(plainText));
  return HexUtils::stringToBinary(ret);
}

vector<uint8_t> CryptoUtils::aesDecrypt(const vector<uint8_t> &key, const vector<uint8_t> &iv, const vector<uint8_t> &encryptedText)
{
  string ret = AESEncryption::aesDecrypt(HexUtils::binaryToString(key), HexUtils::binaryToString(iv), HexUtils::binaryToString(encryptedText));
  return HexUtils::stringToBinary(ret);
}
