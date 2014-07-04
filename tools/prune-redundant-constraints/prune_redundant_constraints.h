#ifndef TACO_TOOLS_PRUNE_REDUNDANT_CONSTRAINTS_PRUNE_REDUNDANT_CONSTRAINTS_H_
#define TACO_TOOLS_PRUNE_REDUNDANT_CONSTRAINTS_PRUNE_REDUNDANT_CONSTRAINTS_H_

#include "tools-common/cli/tool.h"

#include "taco/constraint_set.h"
#include "taco/feature_selection_table.h"
#include "taco/base/string_piece.h"
#include "taco/base/vocabulary.h"

#include <boost/shared_ptr.hpp>

#include <string>
#include <utility>
#include <vector>

namespace taco {
namespace tool {

struct Options;

class PruneRedundantConstraints : public Tool {
 public:
  PruneRedundantConstraints() : Tool("prune-redundant-constraints") {}
  virtual int Main(int, char *[]);
 private:
  typedef std::vector<boost::shared_ptr<ConstraintSet> > ConstraintTable;

  bool IsFullyLexical(const std::vector<StringPiece> &,
                      const std::set<int> &) const;

  bool IsNonTerminal(const StringPiece &) const;

  void LoadConstraintTable(std::istream &);

  void LoadFeatureSelectionTable(std::istream &);

  void ProcessOptions(int, char *[], Options &) const;

  void WriteAdjustedRule(const std::string &,
                         const std::vector<std::pair<int, int> > &,
                         int, std::ostream &);

  Vocabulary feature_set_;
  Vocabulary value_set_;
  ConstraintTable constraint_table_;
  FeatureSelectionTable feature_selection_table_;
};

}  // namespace tool
}  // namespace taco

#endif
