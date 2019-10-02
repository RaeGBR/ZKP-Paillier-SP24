#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include "../vendor/json/json.hpp"

namespace cryptoplus
{
} // namespace cryptoplus

using namespace std;
using namespace cryptoplus;
using json = nlohmann::json;

typedef std::vector<uint8_t> binary_t;

#include "Utils.hpp"
#include "HashUtils.hpp"
#include "./utils/HexUtils.hpp"
