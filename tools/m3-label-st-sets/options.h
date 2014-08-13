#ifndef TACO_TOOLS_M3_LABEL_ST_SETS_OPTIONS_H_
#define TACO_TOOLS_M3_LABEL_ST_SETS_OPTIONS_H_

#include <string>

namespace taco {
namespace tool {
namespace m3 {

struct Options {
 public:
  Options() : subject_verb(true) {}

  // Positional options.
  std::string input_file;

  // Other options.
  std::string output_file;
  bool subject_verb;
};

}  // namespace m3
}  // namespace tool
}  // namespace taco

#endif
