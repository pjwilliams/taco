#include "tree_fragment.h"

namespace taco {
namespace tool {
namespace m3 {

bool TreeFragment::Contains(const Tree *node, bool allow_leaf) const {
  for (std::vector<Tree *>::const_iterator p = leaves.begin();
       p != leaves.end(); ++p) {
    Tree *t = *p;
    if (t == node) {
      return allow_leaf;
    }
    while (t != root) {
      t = t->parent();
      assert(t);
      if (t == node) {
        return true;
      }
    }
  }
  return false;
}

}  // namespace m3
}  // namespace tool
}  // namespace taco
