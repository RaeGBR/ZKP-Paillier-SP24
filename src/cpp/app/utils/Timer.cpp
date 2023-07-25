#include "./Timer.hpp"

map<string, high_resolution_clock::time_point> Timer::t1;
map<string, high_resolution_clock::time_point> Timer::t2;

void Timer::start(const string &name)
{
  Timer::t1[name] = high_resolution_clock::now();
}

double Timer::end(const string &name, bool quite)
{
  Timer::t2[name] = high_resolution_clock::now();
  double tDiff = duration_cast<milliseconds>(Timer::t2[name] - Timer::t1[name]).count();
  tDiff /= 1000;
  if (!quite)
  {
    cout << name << " time: " << tDiff << endl;
  }
  return tDiff;
}

// nanosecond
double Timer::endNan(const string &name, bool quite)
{
  Timer::t2[name] = high_resolution_clock::now();
  double tDiff = duration_cast<nanoseconds>(Timer::t2[name] - Timer::t1[name]).count();
  if (!quite)
  {
    cout << name << " time: " << tDiff << endl;
  }
  return tDiff;
}
