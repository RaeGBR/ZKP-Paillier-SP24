#pragma once

#include "CryptoUtils.hpp"

using namespace cryptoplus;

#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(CryptoUtils)
{
  emscripten::class_<CryptoUtils>("CryptoUtils")
      .class_function("ecGenerateSignature", &CryptoUtils::ecGenerateSignature)
      .class_function("ecVerifySignature", &CryptoUtils::ecVerifySignature)
      .class_function("ecEncrypt", &CryptoUtils::ecEncrypt)
      .class_function("ecDecrypt", &CryptoUtils::ecDecrypt)
      .class_function("aesEncrypt", &CryptoUtils::aesEncrypt)
      .class_function("aesDecrypt", &CryptoUtils::aesDecrypt);
}
