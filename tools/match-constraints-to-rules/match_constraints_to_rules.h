#ifndef TACO_TOOLS_MATCH_CONSTRAINTS_TO_RULES_MATCH_CONSTRAINTS_TO_RULES_H_
#define TACO_TOOLS_MATCH_CONSTRAINTS_TO_RULES_MATCH_CONSTRAINTS_TO_RULES_H_

#include "tools-common/cli/tool.h"

#include "taco/base/vocabulary.h"

#include <string>
#include <utility>
#include <vector>

namespace taco {
namespace tool {

struct Options;

class MatchConstraintsToRules : public Tool {
 public:
  MatchConstraintsToRules() : Tool("match-constraints-to-rules") {}
  virtual int Main(int, char *[]);
 private:
  template<typename Parser> struct InputReader;
  void ProcessOptions(int, char *[], Options &) const;
};

}  // namespace tool
}  // namespace taco

#endif
