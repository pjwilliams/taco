#ifndef TACO_TOOLS_REPAIR_LEXICON_OPTIONS_H_
#define TACO_TOOLS_REPAIR_LEXICON_OPTIONS_H_

#include <string>

namespace taco {
namespace tool {

struct Options {
 public:
  Options() {}
  std::string input_file;
  std::string output_file;
  std::string replace_file;
};

}  // namespace tool
}  // namespace taco

#endif
