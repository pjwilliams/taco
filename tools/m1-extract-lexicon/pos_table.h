#ifndef TACO_TOOLS_M1_EXTRACT_LEXICON_POS_TABLE_H_
#define TACO_TOOLS_M1_EXTRACT_LEXICON_POS_TABLE_H_

#include "tools-common/compat-nlp-de/stts.h"

#include "taco/base/vocabulary.h"

#include <boost/unordered_map.hpp>

#include <istream>
#include <set>

namespace taco {
namespace tool {
namespace m1 {

typedef boost::unordered_map<size_t, std::set<de::stts::Tag> > PosTable;

class PosTableLoader {
 public:
  PosTableLoader(Vocabulary &vocab) : vocab_(vocab) {}
  void Load(std::istream &, PosTable &);
 private:
  Vocabulary &vocab_;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
