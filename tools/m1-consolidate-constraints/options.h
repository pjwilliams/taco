#ifndef TACO_TOOLS_M1_CONSOLIDATE_CONSTRAINTS_OPTIONS_H_
#define TACO_TOOLS_M1_CONSOLIDATE_CONSTRAINTS_OPTIONS_H_

#include <string>

namespace taco {
namespace tool {
namespace m1 {

struct Options {
 public:
  Options()
      : majority(0.51)
      , minority(0.05)  // TODO Make this configurable
      , max_conflict_reports(-1) {}

  // Positional options.
  std::string extract_file;
  std::string vocab_file;
  std::string table_file;
  std::string map_file;

  // Other options.
  float majority;
  float minority;
  int max_conflict_reports;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
