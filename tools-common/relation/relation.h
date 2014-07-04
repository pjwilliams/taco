#ifndef TACO_TOOLS_COMMON_RELATION_H_
#define TACO_TOOLS_COMMON_RELATION_H_

#include <set>

namespace taco {
namespace tool {

template<typename NodeType>
struct Relation {
 public:
  typedef std::set<NodeType> NodeSet;

  typedef typename NodeSet::iterator Iterator;
  typedef typename NodeSet::const_iterator ConstIterator;

  Iterator Begin() { return nodes.begin(); }
  Iterator End() { return nodes.end(); }

  ConstIterator Begin() const { return nodes.begin(); }
  ConstIterator End() const { return nodes.end(); }

  bool IsEmpty() const { return nodes.empty(); }
  std::size_t Size() const { return nodes.size(); }

  // TODO Make this private
  NodeSet nodes;

  bool operator<(const Relation &) const;
};

template<typename NodeType>
bool Relation<NodeType>::operator<(const Relation<NodeType> &other) const {
  return nodes < other.nodes;
}

}  // namespace tool
}  // namespace taco

#endif
