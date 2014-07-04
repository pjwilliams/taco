#ifndef TACO_TOOLS_COMBINE_CONSTRAINT_MAPS_OPTIONS_H_
#define TACO_TOOLS_COMBINE_CONSTRAINT_MAPS_OPTIONS_H_

#include <string>
#include <vector>

namespace taco {
namespace tool {

struct Options {
 public:
  // Positional options.
  std::vector<std::string> input_files;

  // Other options.
  std::string output_file;
};

}  // namespace tool
}  // namespace taco

#endif
