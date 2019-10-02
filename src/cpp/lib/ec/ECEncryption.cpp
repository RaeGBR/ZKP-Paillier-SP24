#include "ECEncryption.hpp"

vector<uint8_t> ECEncryption::encrypt(const shared_ptr<ECCurve> &curve, const vector<uint8_t> &publicKey, const vector<uint8_t> &plaintext)
{
  // Retrieve Public Key from Hex Value
  CryptoPP::StringSource pubString(publicKey.data(), publicKey.size(), true);
  CryptoPP::ECIES<CryptoPP::ECP>::Encryptor encryptor(pubString);

  // Calculate the length of cipher text
  size_t textSize = encryptor.CiphertextLength(plaintext.size());
  string cipher;
  cipher.resize(textSize);

  // Encryption
  CryptoPP::RandomPool rnd;
  encryptor.Encrypt(rnd, (CryptoPP::byte *)plaintext.data(), plaintext.size(), (CryptoPP::byte *)(cipher.data()));

  return HexUtils::stringToBinary(cipher);
}

vector<uint8_t> ECEncryption::decrypt(const shared_ptr<ECCurve> &curve, const vector<uint8_t> &privateKey, const vector<uint8_t> &cyphertext)
{
  // Retrieve Private Key from Hex Value
  CryptoPP::StringSource privString(privateKey.data(), privateKey.size(), true);
  CryptoPP::ECIES<CryptoPP::ECP>::Decryptor decryptor(privString);

  // Calculate the length of plain text
  size_t textSize = decryptor.MaxPlaintextLength(cyphertext.size());
  string message;
  message.resize(textSize);

  // Decryption
  CryptoPP::RandomPool rnd;
  decryptor.Decrypt(rnd, (CryptoPP::byte *)cyphertext.data(), cyphertext.size(), (CryptoPP::byte *)message.data());

  return HexUtils::stringToBinary(message);
}
