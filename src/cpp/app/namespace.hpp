#pragma once

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "../vendor/json/json.hpp"

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/vector.h>
#include <NTL/matrix.h>

namespace polyu
{
} // namespace polyu

using namespace std;
using json = nlohmann::json;
using namespace NTL;
using namespace polyu;

typedef std::vector<uint8_t> binary_t;
