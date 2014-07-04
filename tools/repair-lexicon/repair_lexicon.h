#ifndef TACO_TOOLS_REPAIR_LEXICON_REPAIR_LEXICON_H_
#define TACO_TOOLS_REPAIR_LEXICON_REPAIR_LEXICON_H_

#include "tools-common/cli/tool.h"

namespace taco {
namespace tool {

struct Options;

class RepairLexicon : public Tool {
 public:
  RepairLexicon() : Tool("repair-lexicon") {}
  virtual int Main(int, char *[]);
 private:
  void ProcessOptions(int, char *[], Options &) const;
};

}  // namespace tool
}  // namespace taco

#endif
