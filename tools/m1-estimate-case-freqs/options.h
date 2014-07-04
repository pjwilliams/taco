#ifndef TACO_TOOLS_M1_ESTIMATE_CASE_MODEL_OPTIONS_H_
#define TACO_TOOLS_M1_ESTIMATE_CASE_MODEL_OPTIONS_H_

#include "tools-common/m1/parse_tree_type.h"

#include <string>

namespace taco {
namespace tool {
namespace m1 {

struct Options {
 public:
  Options() : tree_type(kBitPar) {}

  // Positional options.
  std::string corpus_file;
  std::string lexicon_file;

  // Other options.
  std::string output_file;
  ParseTreeType tree_type;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
