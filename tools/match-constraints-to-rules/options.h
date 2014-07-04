#ifndef TACO_TOOLS_MATCH_CONSTRAINTS_TO_RULES_OPTIONS_H_
#define TACO_TOOLS_MATCH_CONSTRAINTS_TO_RULES_OPTIONS_H_

#include <string>

namespace taco {
namespace tool {

struct Options {
 public:
  Options() {}
  std::string constraint_map_file;
  std::string output_file;
  std::string rule_table_index_file;
};

}  // namespace tool
}  // namespace taco

#endif
