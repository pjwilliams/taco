#ifndef TACO_TOOLS_COMBINE_CONSTRAINT_MAPS_COMBINE_CONSTRAINT_MAPS_H_
#define TACO_TOOLS_COMBINE_CONSTRAINT_MAPS_COMBINE_CONSTRAINT_MAPS_H_

#include "input_reader.h"

#include "tools-common/cli/tool.h"

#include "taco/base/string_piece.h"

#include <ostream>
#include <vector>

namespace taco {
namespace tool {

struct Options;

class CombineConstraintMaps : public Tool {
 public:
  CombineConstraintMaps() : Tool("combine-constraint-maps") {}
  virtual int Main(int, char *[]);
 private:
  void ProcessOptions(int, char *[], Options &) const;
  void CombineEntries(const std::vector<InputReader *> &,
                      const std::vector<int> &, std::ostream &) const;
  void WriteIds(const std::vector<StringPiece> &, int, std::ostream &) const;
};

}  // namespace tool
}  // namespace taco

#endif
