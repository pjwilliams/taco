#ifndef TACO_SRC_TACO_CONSTRAINT_SET_H_
#define TACO_SRC_TACO_CONSTRAINT_SET_H_

#include <algorithm>
#include <set>

#include <boost/container/flat_set.hpp>
#include <boost/shared_ptr.hpp>

#include "taco/constraint.h"
#include "taco/base/utility.h"

namespace taco {

// TODO templates

class AbsConstraintSet {
 public:
  typedef boost::shared_ptr<const AbsConstraint> Key;
  typedef DereferencingOrderer<Key, AbsConstraintOrderer> KeyOrderer;
  typedef DereferencingOrderer<Key, AbsConstraintEqualityPred> KeyEqualityPred;

 private:
  typedef boost::container::flat_set<Key, KeyOrderer> Set;

 public:
  typedef Set::iterator Iterator;
  typedef Set::const_iterator ConstIterator;

  AbsConstraintSet() {}

  ConstIterator Begin() const { return set_.begin(); }
  ConstIterator End() const { return set_.end(); }

  bool IsEmpty() const { return set_.empty(); }
  size_t Size() const { return set_.size(); }

  void Clear() { set_.clear(); }

  void Reserve(std::size_t c) { set_.reserve(c); }

  std::pair<Iterator, bool> Insert(const Key &k) { return set_.insert(k); }

  ConstIterator Find(const Key &k) const { return set_.find(k); }

  bool ContainsIndex(int) const;

  void GetIndices(std::set<int> &) const;

  bool ContainsRoot() const;

  // FIXME Don't do this inline
  AbsConstraintSet &operator=(const AbsConstraintSet &other) {
    if (&other != this) {
      set_ = other.set_;
    }
    return *this;
  }

  friend bool operator==(const AbsConstraintSet &, const AbsConstraintSet &);
  friend bool operator!=(const AbsConstraintSet &, const AbsConstraintSet &);

 private:
  Set set_;
};

class RelConstraintSet {
 public:
  typedef boost::shared_ptr<const RelConstraint> Key;
  typedef DereferencingOrderer<Key, RelConstraintOrderer> KeyOrderer;
  typedef DereferencingOrderer<Key, RelConstraintEqualityPred> KeyEqualityPred;

 private:
  typedef boost::container::flat_set<Key, KeyOrderer> Set;

 public:
  typedef Set::iterator Iterator;
  typedef Set::const_iterator ConstIterator;

  RelConstraintSet() {}

  ConstIterator Begin() const { return set_.begin(); }
  ConstIterator End() const { return set_.end(); }

  bool IsEmpty() const { return set_.empty(); }
  size_t Size() const { return set_.size(); }

  void Clear() { set_.clear(); }

  void Reserve(std::size_t c) { set_.reserve(c); }

  std::pair<Iterator, bool> Insert(const Key &k) { return set_.insert(k); }

  ConstIterator Find(const Key &k) const { return set_.find(k); }

  bool ContainsIndex(int) const;

  void GetIndices(std::set<int> &) const;

  bool ContainsRoot() const;

  RelConstraintSet &operator=(const RelConstraintSet &other) {
    if (&other != this) {
      set_ = other.set_;
    }
    return *this;
  }

  friend bool operator==(const RelConstraintSet &, const RelConstraintSet &);
  friend bool operator!=(const RelConstraintSet &, const RelConstraintSet &);

 private:
  Set set_;
};

class VarConstraintSet {
 public:
  typedef boost::shared_ptr<const VarConstraint> Key;
  typedef DereferencingOrderer<Key, VarConstraintOrderer> KeyOrderer;
  typedef DereferencingOrderer<Key, VarConstraintEqualityPred> KeyEqualityPred;

 private:
  typedef boost::container::flat_set<Key, KeyOrderer> Set;

 public:
  typedef Set::iterator Iterator;
  typedef Set::const_iterator ConstIterator;

  VarConstraintSet() {}

  ConstIterator Begin() const { return set_.begin(); }
  ConstIterator End() const { return set_.end(); }

  bool IsEmpty() const { return set_.empty(); }
  size_t Size() const { return set_.size(); }

  void Clear() { set_.clear(); }

  void Reserve(std::size_t c) { set_.reserve(c); }

  std::pair<Iterator, bool> Insert(const Key &k) { return set_.insert(k); }

  ConstIterator Find(const Key &k) const { return set_.find(k); }

  bool ContainsIndex(int) const;

  void GetIndices(std::set<int> &) const;

  bool ContainsRoot() const;

  VarConstraintSet &operator=(const VarConstraintSet &other) {
    if (&other != this) {
      set_ = other.set_;
    }
    return *this;
  }

  friend bool operator==(const VarConstraintSet &, const VarConstraintSet &);
  friend bool operator!=(const VarConstraintSet &, const VarConstraintSet &);

 private:
  Set set_;
};

class ConstraintSet {
 public:
  ConstraintSet() {}

  AbsConstraintSet &abs_set() { return abs_set_; }
  const AbsConstraintSet &abs_set() const { return abs_set_; }

  RelConstraintSet &rel_set() { return rel_set_; }
  const RelConstraintSet &rel_set() const { return rel_set_; }

  VarConstraintSet &var_set() { return var_set_; }
  const VarConstraintSet &var_set() const { return var_set_; }

  bool IsEmpty() const;
  size_t Size() const;

  void Clear();

  void Insert(boost::shared_ptr<Constraint>, ConstraintType);

  bool ContainsIndex(int) const;

  void GetIndices(std::set<int> &) const;

  bool ContainsRoot() const;

  int NumVarConstraints() const;

  float MaxProbability() const;

  friend bool operator==(const ConstraintSet &, const ConstraintSet &);
  friend bool operator!=(const ConstraintSet &, const ConstraintSet &);

 private:
  AbsConstraintSet abs_set_;
  RelConstraintSet rel_set_;
  VarConstraintSet var_set_;
};

class AbsConstraintSetEqualityPred {
 public:
  bool operator()(const AbsConstraintSet &a, const AbsConstraintSet &b) const {
    typedef AbsConstraintSet::KeyEqualityPred EqualityPred;
    if (a.Size() != b.Size()) {
      return false;
    }
    return std::equal(a.Begin(), a.End(), b.Begin(), EqualityPred());
  }
};

class RelConstraintSetEqualityPred {
 public:
  bool operator()(const RelConstraintSet &a, const RelConstraintSet &b) const {
    typedef RelConstraintSet::KeyEqualityPred EqualityPred;
    if (a.Size() != b.Size()) {
      return false;
    }
    return std::equal(a.Begin(), a.End(), b.Begin(), EqualityPred());
  }
};

class VarConstraintSetEqualityPred {
 public:
  bool operator()(const VarConstraintSet &a, const VarConstraintSet &b) const {
    typedef VarConstraintSet::KeyEqualityPred EqualityPred;
    if (a.Size() != b.Size()) {
      return false;
    }
    return std::equal(a.Begin(), a.End(), b.Begin(), EqualityPred());
  }
};

class AbsConstraintSetOrderer {
 public:
  bool operator()(const AbsConstraintSet &a, const AbsConstraintSet &b) const {
    typedef DereferencingOrderer<AbsConstraintSet::Key,
                                 AbsConstraintOrderer> Orderer;
    return std::lexicographical_compare(a.Begin(), a.End(),
                                        b.Begin(), b.End(), Orderer());
  }
};

class RelConstraintSetOrderer {
 public:
  bool operator()(const RelConstraintSet &a, const RelConstraintSet &b) const {
    typedef DereferencingOrderer<RelConstraintSet::Key,
                                 RelConstraintOrderer> Orderer;
    return std::lexicographical_compare(a.Begin(), a.End(),
                                        b.Begin(), b.End(), Orderer());
  }
};

class VarConstraintSetOrderer {
 public:
  bool operator()(const VarConstraintSet &a, const VarConstraintSet &b) const {
    typedef DereferencingOrderer<VarConstraintSet::Key,
                                 VarConstraintOrderer> Orderer;
    return std::lexicographical_compare(a.Begin(), a.End(),
                                        b.Begin(), b.End(), Orderer());
  }
};

class ConstraintSetOrderer {
 public:
  bool operator()(const ConstraintSet &a, const ConstraintSet &b) const {
    AbsConstraintSetEqualityPred abs_set_equal;
    if (!abs_set_equal(a.abs_set(), b.abs_set())) {
      AbsConstraintSetOrderer orderer;
      return orderer(a.abs_set(), b.abs_set());
    }
    RelConstraintSetEqualityPred rel_set_equal;
    if (!rel_set_equal(a.rel_set(), b.rel_set())) {
      RelConstraintSetOrderer orderer;
      return orderer(a.rel_set(), b.rel_set());
    }
    VarConstraintSetOrderer orderer;
    return orderer(a.var_set(), b.var_set());
  }
};

}  // namespace taco

#endif
