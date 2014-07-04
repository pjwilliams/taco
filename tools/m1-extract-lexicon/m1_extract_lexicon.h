#ifndef TACO_TOOLS_M1_EXTRACT_LEXICON_EXTRACT_LEXICON_H_
#define TACO_TOOLS_M1_EXTRACT_LEXICON_EXTRACT_LEXICON_H_

#include "tools-common/cli/tool.h"

namespace taco {
namespace tool {
namespace m1 {

struct Options;

class ExtractLexicon : public Tool {
 public:
  ExtractLexicon() : Tool("m1-extract-lexicon") {}
  virtual int Main(int, char *[]);
 private:
  void ProcessOptions(int, char *[], Options &) const;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
