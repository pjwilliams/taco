#ifndef TACO_TOOLS_M1_EXTRACT_CONSTRAINTS_TREE_CONTEXT_BITPAR_H_
#define TACO_TOOLS_M1_EXTRACT_CONSTRAINTS_TREE_CONTEXT_BITPAR_H_

#include "tree_context.h"

#include "tools-common/compat-nlp-de/bitpar.h"

namespace taco {
namespace tool {
namespace m1 {

// Provides information about the tree from which a grammar rule was extracted.
class TreeContextBitPar : public TreeContext {
 public:
  TreeContextBitPar(const CaseTable *);

  std::string GetNormalizedPOS(const Tree &) const;

 protected:
  const Tree *HasStrongDeclension(const Relation &) const;
  bool GetCaseModelKey(const std::string &, std::string &) const;

 private:
  de::BitParLabelParser label_parser_;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
