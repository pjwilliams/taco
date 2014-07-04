#ifndef TACO_TOOLS_M1_LABEL_ST_SETS_LABEL_RELATIONS_H_
#define TACO_TOOLS_M1_LABEL_ST_SETS_LABEL_RELATIONS_H_

#include "typedef.h"

#include "tools-common/cli/tool.h"

#include <vector>

namespace taco {
namespace tool {
namespace m1 {

struct Options;

class LabelSTSets : public Tool {
 public:
  LabelSTSets() : Tool("m1-label-st-sets") {}
  virtual int Main(int argc, char *argv[]);
 private:
  void ProcessOptions(int, char *[], Options &) const;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
