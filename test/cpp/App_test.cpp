#include "gtest/gtest.h"

#include <fstream>

#include "app/namespace.hpp"

#include "app/CBase.hpp"
#include "app/CEnc.hpp"
#include "app/CBatchEnc.hpp"
#include "app/PaillierEncryption.hpp"
#include "app/CircuitZKPVerifier.hpp"
#include "app/CircuitZKPProver.hpp"
#include "app/math/Matrix.hpp"
#include "app/utils/Timer.hpp"

#include "app/App.hpp"
#include "app/EndToEnd.hpp"


namespace
{

//*/
TEST(App, Run)
{
  string filename = "../../end_to_end(1024,1-400).csv";
  ifstream ifile(filename);
  ofstream fs;
  if (ifile)
  {
    // ifile.cloes();
    fs.open(filename, std::ios_base::app);
  }
  else
  {
    // ifile.cloes();
    fs.open(filename);
    fs << "message count,";
    fs << "message/batch,";
    fs << "batch count,";
    fs << "byte length(bit),";
    fs << "byte length(byte),";
    fs << "message size(byte),";
    fs << "slot size(byte),";
    fs << "slot/message,";
    fs << "range proof count,";
    fs << "cipher size,";
    fs << "proof size,";
    fs << "proof size/message,";

    fs << "single encrypt circuit's N,";
    fs << "single encrypt circuit's Q,";
    fs << "batch encrypt circuit's N,";
    fs << "batch encrypt circuit's Q,";
    fs << "batch encrypt circuit's matrix m,";
    fs << "batch encrypt circuit's matrix n,";

    fs << "encryption time,";
    fs << "circuit create time,";
    fs << "value assign time,";
    fs << "commit time,";
    fs << "prove time,";
    fs << "verify time" << ",";

    // end-to-end
    fs << "attach time-V(s)" << ",";
    fs << "phase1 time-P(s)" << ",";
    fs << "phase2 time-V(s)" << ",";
    fs << "phase3 time-P(s)" << ",";
    fs << "phase3 time-P(ms)" << ",";

    //amortized time
    fs << "amo. cmt time (ms)" << ",";
    fs << "amo. totalPrf time (ms)" << ",";
    fs << "amo. vrf. time (ms)" << endl;
  }

  size_t byteLength, msgCount, rangeProofCount, slotSize, msgPerBatch;

  byteLength = 8; // bytes
  msgCount = 10;
  rangeProofCount = 80; // sp
  slotSize = 4; 
  msgPerBatch = 15;

  // vector<size_t> bls({8, 16, 32, 64, 128, 256});
  // vector<size_t> ms({10, 20, 50, 100, 200});
  // vector<size_t> bls({64});
  // vector<size_t> bls({128}); // msg length
  vector<size_t> bls({16}); // msg length
  vector<size_t> ms({1, 10, 20, 40, 80, 100, 200, 400});
  // vector<size_t> bls({64, 128});
  // vector<size_t> ms({300, 400, 500, 600, 700, 800, 900, 1000});

  //new for end_to_end
   for (size_t i = 0; i < bls.size(); i++)
  {
    byteLength = bls[i];
    auto crypto = make_shared<PaillierEncryption>(byteLength);

    for (size_t j = 0; j < ms.size(); j++)
    {
      msgCount = ms[j];
      polyu::end_to_end(crypto, msgCount, rangeProofCount, slotSize, msgPerBatch, fs);
    }
  }

  fs.close();


  // old 
  // for (size_t i = 0; i < bls.size(); i++)
  // {
  //   byteLength = bls[i];
  //   auto crypto = make_shared<PaillierEncryption>(byteLength);

  //   for (size_t j = 0; j < ms.size(); j++)
  //   {
  //     msgCount = ms[j];
  //     polyu::run(crypto, msgCount, rangeProofCount, slotSize, msgPerBatch, fs);
  //   }
  // }

  // fs.close();

}
//*/

} // namespace
