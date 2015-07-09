#include "tree_context.h"

#include "typedef.h"

#include "tools-common/relation/relation_tree_ops.h"

namespace taco {
namespace tool {
namespace m3 {

TreeContext::TreeContext(const m1::CaseTable *case_table)
    : case_table_(case_table) {
}

void TreeContext::ResetTree(const Tree &tree) {
  relation_map_.clear();
  ExtractRelations<Tree, kIdxId, RelationMap>(tree, relation_map_);
  if (case_table_) {
    case_model_map_.clear();
    BuildCaseModelMap(tree, case_model_map_);
  }
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
  const m1::CaseTable::ProbabilityFunction *distribution =
    case_table_->Lookup(key);
  if (distribution) {
    int id = root.label().get<kIdxId>().value();
    // TODO check ID isn't already in map
    if (id != -1) {
      map[id] = std::make_pair(&root, distribution);
    }
  }
}

bool TreeContext::GetCaseModelKey(const std::string &label,
                                  std::string &key) const {
  if (label == "subj" || label == "obja" || label == "objd" ||
      label == "objg" || label == "pred" || label == "gmod"  ||
      label == "gmod_pre" || label == "gmod_post" || label == "pn" ||
      label == "app" || label == "zeit"  || label == "kon_gmod"  ||
      label == "kon_gmod_post"  || label == "kon_gmod_pre" ||
      label == "kon_obja"  || label == "kon_objd"  || label == "kon_objg" ||
      label == "kon_subj" || label == "kon_pred" || label == "kon_zeit" ||
      label == "root" || label == "kon_root") {
    key = label;
    return true;
  }
  return false;
}

}  // namespace m3
}  // namespace tool
}  // namespace taco
