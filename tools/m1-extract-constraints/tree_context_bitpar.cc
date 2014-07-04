#include "tree_context_bitpar.h"

#include "tools-common/compat-nlp-de/stts.h"

namespace taco {
namespace tool {
namespace m1 {

TreeContextBitPar::TreeContextBitPar(const CaseTable *case_table)
    : TreeContext(case_table) {
}

std::string TreeContextBitPar::GetNormalizedPOS(const Tree &t) const {
  const de::stts::TagSet &tag_set = de::stts::TagSet::Instance();
  const std::string &label = t.label().get<kIdxCat>();
  de::BitParLabel parsed_label;
  label_parser_.Parse(label, parsed_label);
  return tag_set.Normalize(parsed_label.cat);
}

const Tree *TreeContextBitPar::HasStrongDeclension(
    const Relation &relation) const {
  int det_count = 0;
  const Tree *root = 0;
  de::BitParLabel label;
  for (Relation::ConstIterator p = relation.Begin(); p != relation.End(); ++p) {
    const Tree *node = *p;
    if (node->IsLeaf()) {
      continue;
    }
    label_parser_.Parse(node->label().get<kIdxCat>(), label);
    if (label.cat == "APPRART" || label.cat == "ART" || label.cat == "PDAT" ||
        label.cat == "PIAT" || label.cat == "PPOSAT" || label.cat == "PWAT") {
      ++det_count;
    } else if (label.cat == "NP" || label.cat == "PP") {
      assert(!root);
      root = node;
    }
  }
  return (root && !det_count) ? root : 0;
}

bool TreeContextBitPar::GetCaseModelKey(const std::string &label,
                                        std::string &key) const {
  de::BitParLabel parsed_label;
  label_parser_.Parse(label, parsed_label);
  if (parsed_label.cat == "NP" && !parsed_label.func.empty()) {
    key = parsed_label.func;
    return true;
  }
  return false;
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
