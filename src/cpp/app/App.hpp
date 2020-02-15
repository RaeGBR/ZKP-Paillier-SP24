#pragma once

#include "../lib/namespace.hpp"

#include <fstream>
#include <stdexcept>
#include <vector>
#include <math.h>

#include "../lib/math/IntegerImpl.hpp"
#include "../lib/utils/Timer.hpp"
#include "../lib/paillier/PaillierEncryption.hpp"
#include "./CBatchEnc.hpp"
#include "./CircuitZKPVerifier.hpp"
#include "./CircuitZKPProver.hpp"

using namespace std;

namespace polyu
{

void run(size_t byteLength, size_t msgCount, size_t rangeProofCount, size_t slotSize, size_t msgPerBatch, ofstream &fs);
void run(const shared_ptr<PaillierEncryption> &crypto, size_t msgCount, size_t rangeProofCount, size_t slotSize, size_t msgPerBatch, ofstream &fs);

} // namespace polyu
