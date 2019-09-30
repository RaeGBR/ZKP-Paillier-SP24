#pragma once

#include "Integer.hpp"

using namespace cryptoplus;

#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(Integer)
{
  emscripten::class_<Integer>("Integer")
      .smart_ptr_constructor("Integer", &Integer::create)
      .class_function("create", &Integer::create)
      .class_function("createWithString", &Integer::createWithString)
      .class_function("createWithBinary", &Integer::createWithBinary)
      .class_function("createWithNumber", &Integer::createWithNumber)
      .class_function("ZERO", &Integer::ZERO)
      .class_function("ONE", &Integer::ONE)
      .class_function("TWO", &Integer::TWO)
      .class_function("pow2", &Integer::pow2)
      .function("eq", &Integer::eq)
      .function("add", &Integer::add)
      .function("sub", &Integer::sub)
      .function("mul", &Integer::mul)
      .function("mod", &Integer::mod)
      .function("inv", &Integer::inv)
      .function("modMul", &Integer::modMul)
      .function("modPow", &Integer::modPow)
      .function("modSqrt", &Integer::modSqrt)
      .function("toString", &Integer::toString)
      .function("toHex", &Integer::toHex)
      .function("toBinary", &Integer::toBinary)
      .function("toNumber", &Integer::toNumber);
}
