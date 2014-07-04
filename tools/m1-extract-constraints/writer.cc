#include "writer.h"

#include "tree_fragment.h"

#include "tools-common/text-formats/constraint_extract_writer.h"

#include "taco/constraint_set_set.h"

namespace taco {
namespace tool {
namespace m1 {

void Writer::Write(const TreeFragment &fragment, const CSVec &constraint_sets) {
  AddBrackets(fragment.root->label().get<kIdxCat>(), lhs_);
  rhs_.clear();
  rhs_.reserve(fragment.leaves.size());
  for (size_t i = 0; i < fragment.leaves.size(); ++i) {
    const Tree &node = *(fragment.leaves[i]);
    if (node.IsLeaf()) {
      rhs_.push_back(node.label().get<kIdxCat>());
    } else {
      rhs_.resize(rhs_.size()+1);
      AddBrackets(node.label().get<kIdxCat>(), rhs_.back());
    }
  }
  extract_writer_.Write(lhs_, rhs_,
                        constraint_sets.begin(), constraint_sets.end());
}

void Writer::AddBrackets(const std::string &s, std::string &t) const {
  t.clear();
  t.reserve(s.size()+2);
  t += "[";
  t += s;
  t += "]";
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
