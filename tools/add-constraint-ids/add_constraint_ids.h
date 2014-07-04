#ifndef TACO_TOOLS_ADD_CONSTRAINT_IDS_ADD_CONSTRAINT_IDS_H_
#define TACO_TOOLS_ADD_CONSTRAINT_IDS_ADD_CONSTRAINT_IDS_H_

#include <string>
#include <utility>
#include <vector>

#include "taco/base/vocabulary.h"

#include "tools-common/cli/tool.h"

namespace taco {
namespace tool {

struct Options;

class AddConstraintIds : public Tool {
 public:
  AddConstraintIds() : Tool("add-constraint-ids") {}
  virtual int Main(int, char *[]);
 private:
  void ProcessOptions(int, char *[], Options &) const;
};

}  // namespace tool
}  // namespace taco

#endif
