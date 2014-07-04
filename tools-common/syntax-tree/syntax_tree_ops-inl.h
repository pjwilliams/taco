#ifndef TACO_TOOLS_COMMON_SYNTAX_TREE_SYNTAX_TREE_OPS_INL_H_
#define TACO_TOOLS_COMMON_SYNTAX_TREE_SYNTAX_TREE_OPS_INL_H_

#include <utility>
#include <vector>

#include <boost/tuple/tuple.hpp>
#include <boost/type_traits.hpp>

#include "tools-common/syntax-tree/syntax_tree.h"

namespace taco {
namespace tool {

namespace internal {

template<typename TreeType, typename MapType>
void BuildSpanMapTopDown(const TreeType &node, MapType &spans) {
  if (node.IsLeaf()) {
    return;
  }
  // Recursively record spans for children first.
  const std::vector<TreeType *> &children = node.children();
  for (typename std::vector<TreeType *>::const_iterator p = children.begin();
       p != children.end(); ++p) {
    BuildSpanMapTopDown(**p, spans);
  }
  // Span begins at start of first child's spans
  Span &span = spans[&node];
  span.first = spans[children[0]].first;
  span.second = spans[children.back()].second;
}

}  // namespace internal

template<typename TreeType, typename MapType>
void BuildSpanMap(const TreeType &tree, MapType &map) {
  map.clear();
  std::vector<const TreeType *> leaves;
  tree.GetLeaves(leaves);
  for (std::size_t i = 0; i < leaves.size(); ++i) {
    map[leaves[i]] = Span(i, i);
  }
  internal::BuildSpanMapTopDown(tree, map);
}

template<typename TreeType>
void IndexNodesPostOrder(TreeType &node, std::vector<TreeType *> &vec) {
  // Recursively index children first.
  const std::vector<TreeType *> &children = node.children();
  for (typename std::vector<TreeType *>::const_iterator p = children.begin();
       p != children.end(); ++p) {
    IndexNodesPostOrder(**p, vec);
  }
  vec.push_back(&node);
}

template<typename TreeType>
void IndexNodesPostOrder(const TreeType &node,
                         std::vector<const TreeType *> &vec) {
  // Recursively index children first.
  const std::vector<TreeType *> &children = node.children();
  for (typename std::vector<TreeType *>::const_iterator p = children.begin();
       p != children.end(); ++p) {
    IndexNodesPostOrder(**p, vec);
  }
  vec.push_back(&node);
}

template<typename Tree>
void ArticulateLeft(Tree &t, std::size_t n, const typename Tree::Label &label) {
  const std::vector<Tree *> &children = t.children();
  Tree *new_tree = new Tree(label);
  std::vector<Tree *> left_children;
  std::vector<Tree *> right_children;
  left_children.reserve(n);
  right_children.reserve(children.size()-n+1);
  right_children.push_back(new_tree);
  for (std::size_t i = 0; i < children.size(); ++i) {
    Tree *child = children[i];
    if (i < n) {
      left_children.push_back(child);
      child->parent() = new_tree;
    } else {
      right_children.push_back(child);
    }
  }
  new_tree->children() = left_children;
  new_tree->parent() = &t;
  t.children() = right_children;
}

template<typename Tree>
void ArticulateRight(Tree &t, std::size_t n,
                     const typename Tree::Label &label) {
  const std::vector<Tree *> &children = t.children();
  const std::size_t k = children.size() - n;
  Tree *new_tree = new Tree(label);
  std::vector<Tree *> left_children;
  std::vector<Tree *> right_children;
  left_children.reserve(k+1);
  right_children.reserve(n);
  for (std::size_t i = 0; i < children.size(); ++i) {
    Tree *child = children[i];
    if (i < k) {
      left_children.push_back(child);
    } else {
      child->parent() = new_tree;
      right_children.push_back(child);
    }
  }
  left_children.push_back(new_tree);
  new_tree->children() = right_children;
  new_tree->parent() = &t;
  t.children() = left_children;
}

}  // namespace tool
}  // namespace taco

#endif
