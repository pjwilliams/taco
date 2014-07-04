#ifndef TACO_TOOLS_M1_EXTRACT_CONSTRAINTS_TREE_CONTEXT_H_
#define TACO_TOOLS_M1_EXTRACT_CONSTRAINTS_TREE_CONTEXT_H_

#include "typedef.h"

#include "tools-common/m1/case_model.h"

namespace taco {
namespace tool {
namespace m1 {

// Provides information about the tree from which a grammar rule was extracted.
class TreeContext {
 public:
  TreeContext(const CaseTable *);
  virtual ~TreeContext() {}

  void ResetTree(const Tree &);

  const CaseModelMap *GetCaseModelMap() const {
    return case_table_ ? &case_model_map_ : 0;
  }

  // Given a preterminal tree node, returns the normalized part-of-speech tag.
  virtual std::string GetNormalizedPOS(const Tree &) const = 0;

  // Given the ID of an selector-target relation, checks the relation for an NP
  // or a PP with no determiners, returning its root if found or 0 otherwise.
  // The result is cached and the check is only actually performed if the
  // result is unknown.
  const Tree *HasStrongDeclension(int) const;

 protected:
  // As above, but no caching.
  // Implemented by a tree type-specific subclass (BitPar or ParZu).
  virtual const Tree *HasStrongDeclension(const Relation &) const = 0;

  // Extracts the case model key from a tree label.
  // Implemented by a tree type-specific subclass (BitPar or ParZu).
  virtual bool GetCaseModelKey(const std::string &, std::string &) const = 0;

 private:
  typedef std::map<int, const Tree *> StrongDeclMap;

  void BuildCaseModelMap(const Tree &root, CaseModelMap &map) const;

  RelationMap relation_map_;
  mutable StrongDeclMap strong_decl_cache_;
  const CaseTable *case_table_;
  CaseModelMap case_model_map_;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
