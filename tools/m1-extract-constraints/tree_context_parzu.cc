#include "tree_context_parzu.h"

#include "tools-common/compat-nlp-de/stts.h"

namespace taco {
namespace tool {
namespace m1 {

TreeContextParZu::TreeContextParZu(const CaseTable *case_table)
    : TreeContext(case_table) {
}

std::string TreeContextParZu::GetNormalizedPOS(const Tree &t) const {
  const de::stts::TagSet &tag_set = de::stts::TagSet::Instance();
  const std::string &label = t.label().get<kIdxCat>();
  return tag_set.Normalize(label);
}

const Tree *TreeContextParZu::HasStrongDeclension(
    const Relation &relation) const {
  int det_count = 0;
  const Tree *root = 0;
  for (Relation::ConstIterator p = relation.Begin(); p != relation.End(); ++p) {
    const Tree *node = *p;
    if (node->IsLeaf()) {
      continue;
    }
    const std::string &cat = node->label().get<kIdxCat>();
    if (cat == "APPRART" || cat == "ART" || cat == "PDAT" || cat == "PIAT" ||
        cat == "PIDAT" || cat == "PPOSAT" || cat == "PWAT"  || cat == "det") {
      ++det_count;
    } else if (cat == "subj" || cat == "obja" || cat == "objd" ||
               cat == "objg" || cat == "pred" || cat == "gmod" ||
               cat == "gmod_pre" || cat == "gmod_post" || cat == "zeit" ||
               cat == "app" || cat == "pp" || cat == "objp") {
      assert(!root);
      root = node;
    }
  }
  return (root && !det_count) ? root : 0;
}

bool TreeContextParZu::GetCaseModelKey(const std::string &label,
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

}  // namespace m1
}  // namespace tool
}  // namespace taco
