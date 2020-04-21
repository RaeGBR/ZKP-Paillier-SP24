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

void run(size_t byteLength, size_t msgCount, size_t rangeProofCount, size_t slotSize, size_t msgPerBatch, ofstream &fs);
void run(const shared_ptr<PaillierEncryption> &crypto, size_t msgCount, size_t rangeProofCount, size_t slotSize, size_t msgPerBatch, ofstream &fs);

} // namespace polyu
