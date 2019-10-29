#pragma once

#include "Utils.hpp"

using namespace cryptoplus;

#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(Utils)
{
  emscripten::class_<Utils>("Utils")
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
