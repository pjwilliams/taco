#ifndef TACO_TOOLS_M1_EXTRACT_CONSTRAINTS_OPTIONS_H_
#define TACO_TOOLS_M1_EXTRACT_CONSTRAINTS_OPTIONS_H_

#include "tools-common/m1/parse_tree_type.h"

#include <string>

namespace taco {
namespace tool {
namespace m1 {

struct Options {
 public:
  Options()
      : case_model_threshold(-1)
      , disable_strong_decl(false)
      , retain_lexical(false)
      , tree_type(kBitPar) {}

  // Positional options.
  std::string corpus_file;
  std::string rule_file;

  // Other options.
  float case_model_threshold;
  std::string case_table_file;
  bool disable_strong_decl;
  std::string output_file;
  bool retain_lexical;
  ParseTreeType tree_type;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
