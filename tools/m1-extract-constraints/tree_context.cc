#include "tree_context.h"

#include "typedef.h"

#include "tools-common/compat-nlp-de/bitpar.h"
#include "tools-common/relation/relation_tree_ops.h"

namespace taco {
namespace tool {
namespace m1 {

TreeContext::TreeContext(const CaseTable *case_table)
    : case_table_(case_table) {
}

void TreeContext::ResetTree(const Tree &tree) {
  relation_map_.clear();
  ExtractRelations<Tree, kIdxId, RelationMap>(tree, relation_map_);
  strong_decl_cache_.clear();
  if (case_table_) {
    case_model_map_.clear();
    BuildCaseModelMap(tree, case_model_map_);
  }
}

const Tree *TreeContext::HasStrongDeclension(int relation_id) const {
  StrongDeclMap::const_iterator p = strong_decl_cache_.find(relation_id);
  if (p != strong_decl_cache_.end()) {
    return p->second;
  }
  RelationMap::const_iterator q = relation_map_.find(relation_id);
  assert(q != relation_map_.end());
  const Relation &relation = q->second;
  const Tree *root = HasStrongDeclension(relation);
  strong_decl_cache_[relation_id] = root;
  return root;
}

// Find all noun phrase relations in tree and look up their case probability
// distributions.
void TreeContext::BuildCaseModelMap(const Tree &root, CaseModelMap &map) const {
  assert(case_table_);

  if (root.IsPreterminal() || root.IsLeaf()) {
    return;
  }

  const std::vector<Tree *> &children = root.children();
  for (std::vector<Tree *>::const_iterator p = children.begin();
       p != children.end(); ++p) {
    Tree &child = **p;
    BuildCaseModelMap(child, map);
  }

  std::string key;
  if (!GetCaseModelKey(root.label().get<kIdxCat>(), key)) {
    return;
  }
  const CaseTable::ProbabilityFunction *distribution = case_table_->Lookup(key);
  if (distribution) {
    int id = root.label().get<kIdxId>().value();
    // TODO check ID isn't already in map
    if (id != -1) {
      map[id] = std::make_pair(&root, distribution);
    }
  }
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
