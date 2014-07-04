#ifndef TACO_TOOLS_PRUNE_REDUNDANT_CONSTRAINTS_OPTIONS_H_
#define TACO_TOOLS_PRUNE_REDUNDANT_CONSTRAINTS_OPTIONS_H_

#include <string>

namespace taco {
namespace tool {

struct Options {
 public:
  Options() {}
  std::string constraint_table_file;
  std::string feature_selection_table_file;
  std::string output_file;
};

}  // namespace tool
}  // namespace taco

#endif
