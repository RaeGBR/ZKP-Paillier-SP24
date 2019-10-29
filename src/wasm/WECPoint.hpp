#pragma once

#include "ECPoint.hpp"

using namespace cryptoplus;

#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(ECPoint)
{
  emscripten::class_<ECPoint>("ECPoint")
      .smart_ptr_constructor("ECPoint", &ECPoint::create)
      .class_function("create", &ECPoint::create)
      .function("getX", &ECPoint::getX)
      .function("getY", &ECPoint::getY)
      .function("setX", &ECPoint::setX)
      .function("setY", &ECPoint::setY)
      .function("eq", &ECPoint::eq)
      .function("add", &ECPoint::add)
      .function("mul", &ECPoint::mul);
}
