#ifndef TACO_TOOLS_M1_EXTRACT_VOCAB_EXTRACT_VOCAB_H_
#define TACO_TOOLS_M1_EXTRACT_VOCAB_EXTRACT_VOCAB_H_

#include "tools-common/cli/tool.h"

namespace taco {
namespace tool {
namespace m1 {

struct Options;

class ExtractVocab : public Tool {
 public:
  ExtractVocab() : Tool("m1-extract-vocab") {}
  virtual int Main(int, char *[]);
 private:
  void ProcessOptions(int, char *[], Options &) const;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
