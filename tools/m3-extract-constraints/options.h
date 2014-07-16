#ifndef TACO_TOOLS_M3_EXTRACT_CONSTRAINTS_OPTIONS_H_
#define TACO_TOOLS_M3_EXTRACT_CONSTRAINTS_OPTIONS_H_

#include <string>

namespace taco {
namespace tool {
namespace m3 {

struct Options {
 public:
  Options()
      : case_model_threshold(-1)
      , retain_lexical(false) {}

  // Positional options.
  std::string corpus_file;
  std::string rule_file;

  // Other options.
  float case_model_threshold;
  std::string case_table_file;
  std::string output_file;
  bool retain_lexical;
};

}  // namespace m3
}  // namespace tool
}  // namespace taco

#endif
