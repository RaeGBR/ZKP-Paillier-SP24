#pragma once

#include "../namespace.hpp"

#include <chrono>

using namespace std::chrono;

namespace polyu
{

class Timer
{
public:
  static map<string, high_resolution_clock::time_point> t1;
  static map<string, high_resolution_clock::time_point> t2;

  static void start(const string &name);
  static double end(const string &name, bool quite = false);
  static double endMil(const string &name, bool quite = false);
};

} // namespace polyu
