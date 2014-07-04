#ifndef TACO_TOOLS_M1_EXTRACT_CONSTRAINTS_TREE_FRAGMENT_H_
#define TACO_TOOLS_M1_EXTRACT_CONSTRAINTS_TREE_FRAGMENT_H_

#include "typedef.h"

#include <vector>

namespace taco {
namespace tool {
namespace m1 {

struct TreeFragment {
 public:
  bool Contains(const Tree *, bool allow_leaf=true) const;

  Tree *root;
  std::vector<Tree *> leaves;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
