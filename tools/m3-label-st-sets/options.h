#ifndef TACO_TOOLS_M3_LABEL_ST_SETS_OPTIONS_H_
#define TACO_TOOLS_M3_LABEL_ST_SETS_OPTIONS_H_

#include <string>

namespace taco {
namespace tool {
namespace m3 {

struct Options {
 public:
  Options() {}

  // Positional options.
  std::string input_file;

  // Other options.
  std::string output_file;
};

}  // namespace m3
}  // namespace tool
}  // namespace taco

#endif
