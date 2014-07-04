#ifndef TACO_TOOLS_INDEX_RULE_TABLE_INDEX_RULE_TABLE_H_
#define TACO_TOOLS_INDEX_RULE_TABLE_INDEX_RULE_TABLE_H_

#include "tools-common/cli/tool.h"

#include "taco/base/vocabulary.h"

#include <string>
#include <utility>
#include <vector>

namespace taco {
namespace tool {

struct Options;

class IndexRuleTable : public Tool {
 public:
  IndexRuleTable() : Tool("index-rule-table") {}
  virtual int Main(int, char *[]);
 private:
  void ProcessOptions(int, char *[], Options &) const;
};

}  // namespace tool
}  // namespace taco

#endif
