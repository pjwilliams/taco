#ifndef TACO_TOOLS_INDEX_RULE_TABLE_OPTIONS_H_
#define TACO_TOOLS_INDEX_RULE_TABLE_OPTIONS_H_

#include <string>

namespace taco {
namespace tool {

struct Options {
 public:
  Options() {}
  std::string output_file;
  std::string table_file;
  std::string vocab_file;
};

}  // namespace tool
}  // namespace taco

#endif
