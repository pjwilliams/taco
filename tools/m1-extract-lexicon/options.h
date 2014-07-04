#ifndef TACO_TOOLS_M1_EXTRACT_LEXICON_OPTIONS_H_
#define TACO_TOOLS_M1_EXTRACT_LEXICON_OPTIONS_H_

#include <string>

namespace taco {
namespace tool {
namespace m1 {

struct Options {
 public:
  Options()
      : disable_decl_feature(false)
      , disable_prep_case(false) {}

  // Positional options.
  std::string analysis_file;
  std::string vocab_file;

  // Other options.
  bool disable_decl_feature;
  bool disable_prep_case;
  std::string output_file;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
