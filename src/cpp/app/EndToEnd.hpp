#pragma once

#include "./namespace.hpp"

#include <fstream>
#include <math.h>

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/vector.h>
#include <NTL/matrix.h>

#include "./utils/Timer.hpp"
#include "./PaillierEncryption.hpp"
#include "./utils/ConvertUtils.hpp"
#include "./math/MathUtils.hpp"
#include "./CBatchEnc.hpp"
#include "./CircuitZKPVerifier.hpp"
#include "./CircuitZKPProver.hpp"

namespace polyu
{

void end_to_end(size_t byteLength, size_t msgCount, size_t rangeProofCount, size_t slotSize, size_t msgPerBatch, ofstream &fs);
void end_to_end(const shared_ptr<PaillierEncryption> &crypto, size_t msgCount, size_t rangeProofCount, size_t slotSize, size_t msgPerBatch, ofstream &fs);

// ZZ_p help_random(ZZ max);
} // namespace polyu
