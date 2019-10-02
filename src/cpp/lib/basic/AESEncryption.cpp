#include "AESEncryption.hpp"

string AESEncryption::aesEncrypt(const string &key, const string &iv, const string &plainText)
{
  if (key.length() > CryptoPP::AES::DEFAULT_KEYLENGTH)
  {
    throw std::invalid_argument("Key cannot be longer than " + CryptoPP::AES::DEFAULT_KEYLENGTH);
  }

  if (iv.length() > CryptoPP::AES::DEFAULT_KEYLENGTH)
  {
    throw std::invalid_argument("IV cannot be longer than " + CryptoPP::AES::DEFAULT_KEYLENGTH);
  }

  std::string ciphertext;
  CryptoPP::AES::Encryption aesEncryption((CryptoPP::byte *)key.c_str(), CryptoPP::AES::DEFAULT_KEYLENGTH);
  CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, (CryptoPP::byte *)iv.c_str());

  CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
  stfEncryptor.Put(reinterpret_cast<const unsigned char *>(plainText.c_str()), plainText.length());
  stfEncryptor.MessageEnd();
  return ciphertext;
}

string AESEncryption::aesDecrypt(const string &key, const string &iv, const string &encryptedText)
{
  if (key.length() > CryptoPP::AES::DEFAULT_KEYLENGTH)
  {
    throw std::invalid_argument("Key cannot be longer than " + CryptoPP::AES::DEFAULT_KEYLENGTH);
  }

  if (iv.length() > CryptoPP::AES::DEFAULT_KEYLENGTH)
  {
    throw std::invalid_argument("IV cannot be longer than " + CryptoPP::AES::DEFAULT_KEYLENGTH);
  }
  std::string decryptedtext;
  CryptoPP::AES::Decryption aesDecryption((CryptoPP::byte *)key.c_str(), CryptoPP::AES::DEFAULT_KEYLENGTH);
  CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, (CryptoPP::byte *)iv.c_str());

  CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedtext));
  stfDecryptor.Put(reinterpret_cast<const unsigned char *>(encryptedText.c_str()), encryptedText.size());
  stfDecryptor.MessageEnd();

  return decryptedtext;
}
