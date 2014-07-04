#include "taco/constraint.h"

#include <cassert>

namespace taco {

void AbsConstraint::GetIndices(std::set<int> &indices) const {
  indices.insert(lhs.index());
}

void RelConstraint::GetIndices(std::set<int> &indices) const {
  indices.insert(lhs.index());
  indices.insert(rhs.index());
}

void VarConstraint::GetIndices(std::set<int> &indices) const {
  indices.insert(lhs.index());
}

bool AbsConstraintEqualityPred::operator()(const AbsConstraint &a,
                                           const AbsConstraint &b) const {
  return a.lhs == b.lhs && a.rhs == b.rhs;
}

bool RelConstraintEqualityPred::operator()(const RelConstraint &a,
                                           const RelConstraint &b) const {
  return a.lhs == b.lhs && a.rhs == b.rhs;
}

bool VarConstraintEqualityPred::operator()(const VarConstraint &a,
                                           const VarConstraint &b) const {
  return a.lhs == b.lhs && a.rhs == b.rhs;
}

bool AbsConstraintOrderer::operator()(const AbsConstraint &a,
                                      const AbsConstraint &b) const {
  if (a.lhs == b.lhs) {
    ValueTermOrderer value_term_orderer;
    return value_term_orderer(a.rhs, b.rhs);
  }
  PathTermOrderer path_term_orderer;
  return path_term_orderer(a.lhs, b.lhs);
}

bool RelConstraintOrderer::operator()(const RelConstraint &a,
                                      const RelConstraint &b) const {
  // FIXME This seems stupid...

  PathTermOrderer path_term_orderer;

  // A relative constraint is an unordered pair of PathTerms.  To compare
  // relative constraints, first determine the correct internal order of
  // their PathTerms...
  const PathTerm *a_min = &a.lhs;
  const PathTerm *a_max = &a.rhs;
  if (path_term_orderer(*a_max, *a_min)) {
    std::swap(a_min, a_max);
  }
  const PathTerm *b_min = &b.lhs;
  const PathTerm *b_max = &b.rhs;
  if (path_term_orderer(*b_max, *b_min)) {
    std::swap(b_min, b_max);
  }

  // ... Then compare the ordered pairs.
  if (*a_min == *b_min) {
    return path_term_orderer(*a_max, *b_max);
  } else {
    return path_term_orderer(*a_min, *b_min);
  }
}

bool VarConstraintOrderer::operator()(const VarConstraint &a,
                                      const VarConstraint &b) const {
  if (a.lhs == b.lhs) {
    VarTermOrderer var_term_orderer;
    return var_term_orderer(a.rhs, b.rhs);
  }
  PathTermOrderer path_term_orderer;
  return path_term_orderer(a.lhs, b.lhs);
}

}  // namespace taco
