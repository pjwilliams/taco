#ifndef TACO_TOOLS_COMMON_SYNTAX_TREE_SYNTAX_TREE_OPS_H_
#define TACO_TOOLS_COMMON_SYNTAX_TREE_SYNTAX_TREE_OPS_H_

#include <utility>
#include <vector>

#include <boost/tuple/tuple.hpp>
#include <boost/type_traits.hpp>

#include "tools-common/syntax-tree/syntax_tree.h"

namespace taco {
namespace tool {

// Represents a contiguous span, where elements are indexed from 0.
typedef std::pair<std::size_t, std::size_t> Span;

// Builds a map from SyntaxTree nodes to Spans.
//
// Tree   A SyntaxTree type
// Map    Map from Tree pointers to Spans
//
template<typename Tree, typename Map>
void BuildSpanMap(const Tree &, Map &);

// Builds an index of SyntaxTree nodes by visiting every node in a postorder
// traversal and assigning contiguous indices (from 0).
//
// Tree   A SyntaxTree type
//
template<typename Tree>
void IndexNodesPostOrder(Tree &, std::vector<Tree *> &);

// Const version of above.
template<typename Tree>
void IndexNodesPostOrder(const Tree &, std::vector<const Tree *> &);

// Given a node t with m children, moves the first n children under a new node
// t', which becomes the leftmost child of t.  The third parameter is the label
// for t'.
template<typename Tree>
void ArticulateLeft(Tree &, std::size_t, const typename Tree::Label &);

// Given a node t with m children, moves the last n children under a new node
// t', which becomes the rightmost child of t.  The third parameter is the label
// for t'.
template<typename Tree>
void ArticulateRight(Tree &, std::size_t, const typename Tree::Label &);

}  // namespace tool
}  // namespace taco

// Implementation.
#include "syntax_tree_ops-inl.h"

#endif
