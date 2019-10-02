#pragma once

#include "KeyPair.hpp"

using namespace cryptoplus;

#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(KeyPair)
{
  emscripten::class_<KeyPair>("KeyPair")
      .smart_ptr_constructor("KeyPair", &KeyPair::create)
      .class_function("create", &KeyPair::create)
      .class_function("createWithSeed", &KeyPair::createWithSeed)
      .class_function("createWithPrivateKey", &KeyPair::createWithPrivateKey)
      .function("getCurve", &KeyPair::getCurve)
      .function("getPrivateKey", &KeyPair::getPrivateKey)
      .function("getPublicKey", &KeyPair::getPublicKey)
      .function("getPublicElement", &KeyPair::getPublicElement)
      .function("getPrivateElement", &KeyPair::getPrivateElement)
      .function("eq", &KeyPair::eq);
}
