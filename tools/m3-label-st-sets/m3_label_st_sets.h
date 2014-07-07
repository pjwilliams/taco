#ifndef TACO_TOOLS_M3_LABEL_ST_SETS_LABEL_RELATIONS_H_
#define TACO_TOOLS_M3_LABEL_ST_SETS_LABEL_RELATIONS_H_

#include <vector>

#include "tools-common/cli/tool.h"

#include "typedef.h"

namespace taco {
namespace tool {
namespace m3 {

struct Options;

class LabelSTSets : public Tool {
 public:
  LabelSTSets() : Tool("m3-label-st-sets") {}
  virtual int Main(int argc, char *argv[]);
 private:
  void ProcessOptions(int, char *[], Options &) const;
};

}  // namespace m3
}  // namespace tool
}  // namespace taco

#endif
