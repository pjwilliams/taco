#ifndef TACO_TOOLS_M1_EXTRACT_CONSTRAINTS_EXTRACT_CONSTRAINTS_H_
#define TACO_TOOLS_M1_EXTRACT_CONSTRAINTS_EXTRACT_CONSTRAINTS_H_

#include "typedef.h"

#include "tools-common/compat-nlp-de/bitpar.h"
#include "tools-common/cli/tool.h"

namespace taco {
namespace tool {
class CaseTable;
namespace m1 {

struct Options;

class ExtractConstraints : public Tool {
 public:
  ExtractConstraints() : Tool("m1-extract-constraints") {}
  virtual int Main(int, char *[]);
 private:
  void ProcessOptions(int, char *[], Options &) const;
  void EnumerateNodes(Tree &, std::vector<Tree *> &) const;
  void ParseLine(const std::string &, int &, std::vector<std::size_t> &) const;

  // TODO Move MergeRelations and AssimilateNpPpRelations into a separate class.
  void MergeRelations(Tree &) const;
  void AssimilateNpPpRelations(Tree &, RelationId, RelationId) const;

  de::BitParLabelParser label_parser_;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
