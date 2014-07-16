#ifndef TACO_TOOLS_M3_EXTRACT_CONSTRAINTS_TREE_FRAGMENT_H_
#define TACO_TOOLS_M3_EXTRACT_CONSTRAINTS_TREE_FRAGMENT_H_

#include "typedef.h"

#include <vector>

namespace taco {
namespace tool {
namespace m3 {

struct TreeFragment {
 public:
  bool Contains(const Tree *, bool allow_leaf=true) const;

  Tree *root;
  std::vector<Tree *> leaves;
};

}  // namespace m3
}  // namespace tool
}  // namespace taco

#endif
