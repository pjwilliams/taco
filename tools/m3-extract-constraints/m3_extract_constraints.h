#ifndef TACO_TOOLS_M3_EXTRACT_CONSTRAINTS_EXTRACT_CONSTRAINTS_H_
#define TACO_TOOLS_M3_EXTRACT_CONSTRAINTS_EXTRACT_CONSTRAINTS_H_

#include "typedef.h"

#include "tools-common/cli/tool.h"

namespace taco {
namespace tool {
class CaseTable;
namespace m3 {

struct Options;

class ExtractConstraints : public Tool {
 public:
  ExtractConstraints() : Tool("m3-extract-constraints") {}
  virtual int Main(int, char *[]);
 private:
  void ProcessOptions(int, char *[], Options &) const;
  void EnumerateNodes(Tree &, std::vector<Tree *> &) const;
  void ParseLine(const std::string &, int &, std::vector<std::size_t> &) const;

  // TODO Move MergeRelations and AssimilateNpPpRelations into a separate class.
  void MergeRelations(Tree &) const;
  void AssimilateNpPpRelations(Tree &, RelationId, RelationId) const;
};

}  // namespace m3
}  // namespace tool
}  // namespace taco

#endif
