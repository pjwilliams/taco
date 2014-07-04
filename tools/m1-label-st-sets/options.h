#ifndef TACO_TOOLS_M1_LABEL_ST_SETS_OPTIONS_H_
#define TACO_TOOLS_M1_LABEL_ST_SETS_OPTIONS_H_

#include "tools-common/m1/parse_tree_type.h"

#include <string>

namespace taco {
namespace tool {
namespace m1 {

struct Options {
 public:
  Options() : subject_verb(false), tree_type(kBitPar) {}

  // Positional options.
  std::string input_file;

  // Other options.
  std::string output_file;
  bool subject_verb;
  ParseTreeType tree_type;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
