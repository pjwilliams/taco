#ifndef TACO_TOOLS_COMMON_SYNTAX_TREE_SYNTAX_TREE_H_
#define TACO_TOOLS_COMMON_SYNTAX_TREE_SYNTAX_TREE_H_

#include <cassert>
#include <vector>

namespace taco {
namespace tool {

template<typename T>
class SyntaxTree {
 public:
  typedef T Label;

  // Constructors
  SyntaxTree()
      : label_()
      , children_()
      , parent_(0) {}

  SyntaxTree(const T &label)
      : label_(label)
      , children_()
      , parent_(0) {}

  SyntaxTree(const T &label, const std::vector<SyntaxTree *> &children)
      : label_(label)
      , children_(children)
      , parent_(0) {}

  // Destructor
  virtual ~SyntaxTree();

  const T &label() const { return label_; }
  T &label() { return label_; }

  const SyntaxTree *parent() const { return parent_; }
  SyntaxTree *&parent() { return parent_; }

  const std::vector<SyntaxTree *> &children() const { return children_; }
  std::vector<SyntaxTree *> &children() { return children_; }

  bool IsLeaf() const { return children_.empty(); }

  bool IsPreterminal() const {
    return children_.size() == 1 && children_[0]->IsLeaf();
  }

  SyntaxTree *GetChild(int i) { return children_[i]; }
  const SyntaxTree *GetChild(int i) const { return children_[i]; }

  void AddChild(SyntaxTree *child) { children_.push_back(child); }

  void GetLeaves(std::vector<SyntaxTree *> &);
  void GetLeaves(std::vector<const SyntaxTree *> &) const;

  void GetYield(std::vector<T> &) const;

  std::size_t YieldSize() const;

  // Calculate the depth of this node within the tree (where the root has a
  // depth of 0, root's children have a depth 1, etc).
  std::size_t Depth() const;

  // Get a vector of ancestors in decreasing depth from parent to root
  void GetAncestors(std::vector<SyntaxTree *> &);
  void GetAncestors(std::vector<const SyntaxTree *> &) const;

 private:
  T label_;
  std::vector<SyntaxTree *> children_;
  SyntaxTree *parent_;
};

// FIXME Version for non-const iterators.
// Find lowest common ancestor of a set of nodes.  A node can count as its
// own ancestor in this context.
template<typename TreeType, typename InputIterator>
const TreeType *LowestCommonAncestor(InputIterator first, InputIterator last) {
  std::vector<std::vector<const TreeType *> > ancestors;

  std::size_t shortest_path_length = 0;
  std::vector<const TreeType *> vec;
  for (std::size_t i = 0; first != last; ++first, ++i) {
    vec.clear();
    (*first)->GetAncestors(vec);
    // Add self as first ancestor.
    vec.insert(vec.begin(), *first);
    ancestors.push_back(vec);
    std::size_t path_length = vec.size();
    assert(path_length > 0);
    if (i == 0 || path_length < shortest_path_length) {
      shortest_path_length = path_length;
    }
  }

  for (std::size_t i = 0; i < shortest_path_length; ++i) {
    const TreeType *lca = 0;
    bool match = true;
    for (std::size_t j = 0; j < ancestors.size(); ++j) {
      std::size_t index = ancestors[j].size() - shortest_path_length + i;
      assert(index >= 0);
      assert(index < ancestors[j].size());
      if (j == 0) {
        lca = ancestors[j][index];
        assert(lca);
      } else if (lca != ancestors[j][index]) {
        match = false;
        break;
      }
    }
    if (match) {
      assert(lca);
      return lca;
    }
  }

  return 0;
}

template<typename T>
SyntaxTree<T>::~SyntaxTree() {
  for (std::size_t i = 0; i < children_.size(); ++i) {
    delete children_[i];
  }
}

template<typename T>
void SyntaxTree<T>::GetLeaves(std::vector<SyntaxTree *> &leaves) {
  if (children_.empty()) {
    leaves.push_back(dynamic_cast<SyntaxTree*>(this));
  } else {
    for (std::size_t i = 0; i < children_.size(); ++i) {
      SyntaxTree *child = children_[i];
      child->GetLeaves(leaves);
    }
  }
}

template<typename T>
void SyntaxTree<T>::GetLeaves(std::vector<const SyntaxTree *> &leaves) const {
  if (children_.empty()) {
    leaves.push_back(dynamic_cast<const SyntaxTree*>(this));
  } else {
    for (std::size_t i = 0; i < children_.size(); ++i) {
      SyntaxTree *child = children_[i];
      child->GetLeaves(leaves);
    }
  }
}

template<typename T>
void SyntaxTree<T>::GetYield(std::vector<T> &yield) const {
  // TODO Should this clear the vector?  Check uses.
  if (IsLeaf()) {
    yield.push_back(label_);
  } else {
    typename std::vector<SyntaxTree *>::const_iterator p(children_.begin());
    typename std::vector<SyntaxTree *>::const_iterator end(children_.end());
    for (; p != end; ++p) {
      const SyntaxTree &child = **p;
      child.GetYield(yield);
    }
  }
}

template<typename T>
std::size_t SyntaxTree<T>::YieldSize() const {
  if (IsLeaf()) {
    return 1;
  }
  std::size_t yield_size = 0;
  typename std::vector<SyntaxTree *>::const_iterator p = children_.begin();
  typename std::vector<SyntaxTree *>::const_iterator end = children_.end();
  for (; p != end; ++p) {
    const SyntaxTree &child = **p;
    yield_size += child.YieldSize();
  }
  return yield_size;
}

template<typename T>
std::size_t SyntaxTree<T>::Depth() const {
  std::size_t depth = 0;
  SyntaxTree *ancestor = parent_;
  while (ancestor != 0) {
    ++depth;
    ancestor = ancestor->parent_;
  }
  return depth;
}

// Get a vector of ancestors, ordered by decreasing depth from parent to root.
template<typename T>
void SyntaxTree<T>::GetAncestors(std::vector<SyntaxTree *> &ancestors) {
  ancestors.clear();
  SyntaxTree *ancestor = parent();
  while (ancestor != 0) {
    ancestors.push_back(ancestor);
    ancestor = ancestor->parent();
  }
}

template<typename T>
void SyntaxTree<T>::GetAncestors(
    std::vector<const SyntaxTree *> &ancestors) const {
  ancestors.clear();
  const SyntaxTree *ancestor = parent();
  while (ancestor != 0) {
    ancestors.push_back(ancestor);
    ancestor = ancestor->parent();
  }
}

}  // namespace tool
}  // namespace taco

#endif
