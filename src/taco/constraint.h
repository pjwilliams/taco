#ifndef TACO_SRC_TACO_CONSTRAINT_H_
#define TACO_SRC_TACO_CONSTRAINT_H_

#include <set>

#include "taco/constraint_term.h"

namespace taco {

enum ConstraintType {
  kAbsConstraint,
  kRelConstraint,
  kVarConstraint
};

struct Constraint {};

struct AbsConstraint : public Constraint {
 public:
  AbsConstraint(const PathTerm &l, const ValueTerm &r) : lhs(l), rhs(r) {}
  AbsConstraint *Clone() const { return new AbsConstraint(*this); }
  void GetIndices(std::set<int> &) const;
  bool ContainsRoot() const { return lhs.index() == 0; }
  bool ContainsIndex(int i) const { return lhs.index() == i; }
  PathTerm lhs;
  ValueTerm rhs;
};

struct RelConstraint : public Constraint {
 public:
  RelConstraint(const PathTerm &l, const PathTerm &r) : lhs(l), rhs(r) {}
  RelConstraint *Clone() const { return new RelConstraint(*this); }
  void GetIndices(std::set<int> &) const;
  bool ContainsRoot() const { return lhs.index() == 0 || rhs.index() == 0; }
  bool ContainsIndex(int i) const { return lhs.index() == i || rhs.index() == i; }
  PathTerm lhs;
  PathTerm rhs;
};

struct VarConstraint : public Constraint {
 public:
  VarConstraint(const PathTerm &l, const VarTerm &r) : lhs(l), rhs(r) {}
  VarConstraint *Clone() const { return new VarConstraint(*this); }
  void GetIndices(std::set<int> &) const;
  bool ContainsRoot() const { return lhs.index() == 0; }
  bool ContainsIndex(int i) const { return lhs.index() == i; }
  float MaxProbability() const { return rhs.MaxProbability(); }
  PathTerm lhs;
  VarTerm rhs;
};

class AbsConstraintEqualityPred {
 public:
  bool operator()(const AbsConstraint &, const AbsConstraint &) const;
};

class RelConstraintEqualityPred {
 public:
  bool operator()(const RelConstraint &, const RelConstraint &) const;
};

class VarConstraintEqualityPred {
 public:
  bool operator()(const VarConstraint &, const VarConstraint &) const;
};

// Defines a (completely arbitrary) strict weak ordering between AbsConstraint
// objects.
class AbsConstraintOrderer {
 public:
  bool operator()(const AbsConstraint &, const AbsConstraint &) const;
};

// Defines a (completely arbitrary) strict weak ordering between RelConstraint
// objects.
class RelConstraintOrderer {
 public:
  bool operator()(const RelConstraint &, const RelConstraint &) const;
};

// Defines a (completely arbitrary) strict weak ordering between VarConstraint
// objects.
class VarConstraintOrderer {
 public:
  bool operator()(const VarConstraint &, const VarConstraint &) const;
};

}  // namespace taco

#endif
