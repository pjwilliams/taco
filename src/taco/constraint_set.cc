#include "taco/constraint_set.h"

namespace taco {

bool AbsConstraintSet::ContainsIndex(int i) const {
  for (ConstIterator p = Begin(); p != End(); ++p) {
    if ((*p)->ContainsIndex(i)) {
      return true;
    }
  }
  return false;
}

bool RelConstraintSet::ContainsIndex(int i) const {
  for (ConstIterator p = Begin(); p != End(); ++p) {
    if ((*p)->ContainsIndex(i)) {
      return true;
    }
  }
  return false;
}

bool VarConstraintSet::ContainsIndex(int i) const {
  for (ConstIterator p = Begin(); p != End(); ++p) {
    if ((*p)->ContainsIndex(i)) {
      return true;
    }
  }
  return false;
}

void AbsConstraintSet::GetIndices(std::set<int> &indices) const {
  for (ConstIterator p = Begin(); p != End(); ++p) {
    (*p)->GetIndices(indices);
  }
}

void RelConstraintSet::GetIndices(std::set<int> &indices) const {
  for (ConstIterator p = Begin(); p != End(); ++p) {
    (*p)->GetIndices(indices);
  }
}

void VarConstraintSet::GetIndices(std::set<int> &indices) const {
  for (ConstIterator p = Begin(); p != End(); ++p) {
    (*p)->GetIndices(indices);
  }
}

bool AbsConstraintSet::ContainsRoot() const {
  for (ConstIterator p = Begin(); p != End(); ++p) {
    if ((*p)->ContainsRoot()) {
      return true;
    }
  }
  return false;
}

bool RelConstraintSet::ContainsRoot() const {
  for (ConstIterator p = Begin(); p != End(); ++p) {
    if ((*p)->ContainsRoot()) {
      return true;
    }
  }
  return false;
}

bool VarConstraintSet::ContainsRoot() const {
  for (ConstIterator p = Begin(); p != End(); ++p) {
    if ((*p)->ContainsRoot()) {
      return true;
    }
  }
  return false;
}

void ConstraintSet::Insert(boost::shared_ptr<Constraint> constraint,
                           ConstraintType type) {
  if (type == kAbsConstraint) {
    boost::shared_ptr<AbsConstraint> abs_constraint =
      boost::static_pointer_cast<AbsConstraint>(constraint);
    abs_set_.Insert(abs_constraint);
  } else if (type == kRelConstraint) {
    boost::shared_ptr<RelConstraint> rel_constraint =
      boost::static_pointer_cast<RelConstraint>(constraint);
    rel_set_.Insert(rel_constraint);
  } else {
    boost::shared_ptr<VarConstraint> var_constraint =
      boost::static_pointer_cast<VarConstraint>(constraint);
    var_set_.Insert(var_constraint);
  }
}

bool ConstraintSet::IsEmpty() const {
  return abs_set_.IsEmpty() && rel_set_.IsEmpty() && var_set_.IsEmpty();
}

std::size_t ConstraintSet::Size() const {
  return abs_set_.Size() + rel_set_.Size() + var_set_.Size();
}

void ConstraintSet::Clear() {
  abs_set_.Clear();
  rel_set_.Clear();
  var_set_.Clear();
}

void ConstraintSet::GetIndices(std::set<int> &indices) const {
  abs_set_.GetIndices(indices);
  rel_set_.GetIndices(indices);
  var_set_.GetIndices(indices);
}

bool ConstraintSet::ContainsIndex(int i) const {
  if (abs_set_.ContainsIndex(i)) {
    return true;
  } else if (rel_set_.ContainsIndex(i)) {
    return true;
  } else {
    return var_set_.ContainsIndex(i);
  }
}

bool ConstraintSet::ContainsRoot() const {
  if (abs_set_.ContainsRoot()) {
    return true;
  } else if (rel_set_.ContainsRoot()) {
    return true;
  } else {
    return var_set_.ContainsRoot();
  }
}

int ConstraintSet::NumVarConstraints() const {
  return var_set_.Size();
}

float ConstraintSet::MaxProbability() const {
  float prob = 1.0f;
  for (VarConstraintSet::ConstIterator p = var_set_.Begin();
       p != var_set_.End(); ++p) {
    prob *= (*p)->MaxProbability();
  }
  return prob;
}

bool operator==(const ConstraintSet &lhs, const ConstraintSet &rhs) {
  if (lhs.Size() != rhs.Size()) {
    return false;
  }
  if (!std::equal(lhs.abs_set_.Begin(), lhs.abs_set_.End(),
                  rhs.abs_set_.Begin(), AbsConstraintSet::KeyEqualityPred())) {
    return false;
  }
  if (!std::equal(lhs.rel_set_.Begin(), lhs.rel_set_.End(),
                  rhs.rel_set_.Begin(), RelConstraintSet::KeyEqualityPred())) {
    return false;
  }
  if (!std::equal(lhs.var_set_.Begin(), lhs.var_set_.End(),
                  rhs.var_set_.Begin(), VarConstraintSet::KeyEqualityPred())) {
    return false;
  }
  return true;
}

bool operator!=(const ConstraintSet &lhs, const ConstraintSet &rhs) {
  return !(lhs == rhs);
}

}  // namespace taco
