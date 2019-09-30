#pragma once

#include "Utils.hpp"

using namespace cryptoplus;

#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(Utils)
{
  emscripten::class_<Utils>("Utils")
      .class_function("calcHash", &Utils::calcHash)
      .class_function("random", &Utils::random)
      .class_function("randomWithSeed", &Utils::randomWithSeed)
      .class_function("randomHex", &Utils::randomHex)
      .class_function("randomHexWithSeed", &Utils::randomHexWithSeed)
      .class_function("randomInt", &Utils::randomInt)
      .class_function("randomIntWithSeed", &Utils::randomIntWithSeed)
      .class_function("calcSignature", &Utils::calcSignature)
      .class_function("verifySignature", &Utils::verifySignature)
      .class_function("ecEncrypt", &Utils::ecEncrypt)
      .class_function("ecDecrypt", &Utils::ecDecrypt)
      .class_function("aesEncrypt", &Utils::aesEncrypt)
      .class_function("aesDecrypt", &Utils::aesDecrypt)
      .class_function("stringToBinary", &Utils::stringToBinary)
      .class_function("binaryToString", &Utils::binaryToString)
      .class_function("stringToHex", &Utils::stringToHex)
      .class_function("hexToBinary", &Utils::hexToBinary)
      .class_function("binaryToHex", &Utils::binaryToHex);

  // register bindings
  emscripten::register_vector<uint8_t>("Binary");
  emscripten::register_vector<std::vector<uint8_t>>("BinaryVector");
  emscripten::register_vector<std::string>("StringVector");
}
