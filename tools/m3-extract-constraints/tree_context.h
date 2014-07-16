#ifndef TACO_TOOLS_M3_EXTRACT_CONSTRAINTS_TREE_CONTEXT_H_
#define TACO_TOOLS_M3_EXTRACT_CONSTRAINTS_TREE_CONTEXT_H_

#include "typedef.h"

#include "tools-common/m1/case_model.h"

namespace taco {
namespace tool {
namespace m3 {

// Provides information about the tree from which a grammar rule was extracted.
class TreeContext {
 public:
  TreeContext(const m1::CaseTable *);

  void ResetTree(const Tree &);

  const CaseModelMap *GetCaseModelMap() const {
    return case_table_ ? &case_model_map_ : 0;
  }

  bool GetCaseModelKey(const std::string &, std::string &) const;

 private:
  void BuildCaseModelMap(const Tree &root, CaseModelMap &map) const;

  RelationMap relation_map_;
  const m1::CaseTable *case_table_;
  CaseModelMap case_model_map_;
};

}  // namespace m3
}  // namespace tool
}  // namespace taco

#endif
