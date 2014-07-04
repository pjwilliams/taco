#ifndef TACO_TOOLS_M1_CONSOLIDATE_CONSTRAINTS_CONSOLIDATE_CONSTRAINTS_H_
#define TACO_TOOLS_M1_CONSOLIDATE_CONSTRAINTS_CONSOLIDATE_CONSTRAINTS_H_

#include "tools-common/cli/tool.h"

namespace taco {
namespace tool {
namespace m1 {

struct Options;
struct Statistics;

class ConsolidateConstraints : public Tool {
 public:
  ConsolidateConstraints() : Tool("m1-consolidate-constraints") {}
  virtual int Main(int, char *[]);
 private:
  void ProcessOptions(int, char *[], Options &) const;
  void PrintCounts(const Statistics &);
  void PrintStats(const Statistics &, int);
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
