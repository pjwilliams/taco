#ifndef TACO_TOOLS_ADD_FEATURE_SELECTION_IDS_OPTIONS_H_
#define TACO_TOOLS_ADD_FEATURE_SELECTION_IDS_OPTIONS_H_

#include <string>

namespace taco {
namespace tool {

struct Options {
 public:
  Options() {}

  // Positional options.
  std::string feature_map_file;
  std::string rule_table_file;

  // Other options.
  std::string output_file;
};

}  // namespace tool
}  // namespace taco

#endif
