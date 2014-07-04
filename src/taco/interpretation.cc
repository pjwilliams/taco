#include "taco/interpretation.h"

#include <algorithm>

namespace taco {

// Hack-y adapter to allow FeatureStructure::MultiClone to be used with
// feature structures stored in an Interpretation::Map.
class Interpretation::MappedValueIterator {
 public:
  MappedValueIterator(Interpretation::Map::const_iterator map_iter)
      : map_iter_(map_iter) {}

  const boost::shared_ptr<FeatureStructure> &operator*() {
    return map_iter_->second;
  }

  MappedValueIterator &operator++() {
    ++map_iter_;
    return *this;
  }

  bool operator!=(MappedValueIterator other) const {
    return map_iter_ != other.map_iter_;
  }

 private:
  Interpretation::Map::const_iterator map_iter_;
};

// Another hack-y adapter for use with FeatureStructure::MultiClone.
class Interpretation::MappedValueInserter {
 public:
  MappedValueInserter(const Interpretation::Map &src_map,
                      Interpretation::Map &dest_map) {
    dest_map = src_map;
    dest_map_iter_ = dest_map.begin();
  }

  boost::shared_ptr<FeatureStructure> &operator*() {
    return dest_map_iter_->second;
  }

  MappedValueInserter &operator++() {
    ++dest_map_iter_;
    return *this;
  }

 private:
  Interpretation::Map::iterator dest_map_iter_;
};

bool PotentialInterpretation::QuickCheck(const ConstraintSet &cs) {
  // If the interpretation already contains a feature structure with the
  // new index then check if the old and new FS are unifiable.
  boost::shared_ptr<const FeatureStructure> prev_fs = prev_.GetFS(index_);
  if (prev_fs.get()) {
    return prev_fs->PossiblyUnifiable(fs_);
  }
  for (AbsConstraintSet::ConstIterator p = cs.abs_set().Begin();
       p != cs.abs_set().End(); ++p) {
    const AbsConstraint &constraint = **p;
    if (!QuickCheck(constraint)) {
      return false;
    }
  }
  for (RelConstraintSet::ConstIterator p = cs.rel_set().Begin();
       p != cs.rel_set().End(); ++p) {
    const RelConstraint &constraint = **p;
    if (!QuickCheck(constraint)) {
        return false;
    }
  }
  for (VarConstraintSet::ConstIterator p = cs.var_set().Begin();
       p != cs.var_set().End(); ++p) {
    const VarConstraint &constraint = **p;
    if (!QuickCheck(constraint)) {
      return false;
    }
  }
  return true;
}

bool PotentialInterpretation::QuickCheck(const AbsConstraint &constraint) {
  if (constraint.lhs.index() != index_) {
    // The constraint doesn't apply to the new feature structure.
    return true;
  }
  const FeaturePath &path = constraint.lhs.path();
  assert(!path.empty());
  AtomicValue atom = constraint.rhs.value();

  boost::shared_ptr<const FeatureStructure> val = fs_->Get(path.begin(),
                                                           path.end());
  if (!val) {
    return true;
  }
  return val->IsAtomic() && val->GetAtomicValue() == atom;
}

bool PotentialInterpretation::QuickCheck(const VarConstraint &constraint) {
  if (constraint.lhs.index() != index_) {
    // The constraint doesn't apply to the new feature structure.
    return true;
  }
  const FeaturePath &path = constraint.lhs.path();
  assert(!path.empty());
  const VarTerm::ProbabilityMap &prob_map = constraint.rhs.probabilities();

  boost::shared_ptr<const FeatureStructure> val = fs_->Get(path.begin(),
                                                           path.end());
  if (!val) {
    return true;
  }
  if (!val->IsAtomic()) {
    return false;
  }
  AtomicValue atom = val->GetAtomicValue();
  return prob_map.find(atom) != prob_map.end();
}

bool PotentialInterpretation::QuickCheck(const RelConstraint &constraint) {
  int lhs_index = constraint.lhs.index();
  int rhs_index = constraint.rhs.index();
  boost::shared_ptr<const FeatureStructure> lhs_root;
  boost::shared_ptr<const FeatureStructure> rhs_root;
  if (index_ == lhs_index) {
    Interpretation::Map::const_iterator p = prev_.values_.find(rhs_index);
    if (p == prev_.values_.end()) {
      // The constraint involves a feature structure that's outside the current
      // range of this interpretation.  Pass by default.
      return true;
    }
    rhs_root = p->second;
    lhs_root = fs_;
  } else if (index_ == rhs_index) {
    Interpretation::Map::const_iterator p = prev_.values_.find(lhs_index);
    if (p == prev_.values_.end()) {
      // The constraint involves a feature structure that's outside the current
      // range of this interpretation.  Pass by default.
      return true;
    }
    lhs_root = p->second;
    rhs_root = fs_;
  } else {
    // The constraint doesn't involve the new feature structure.
    return true;
  }

  const FeaturePath &lhs_path = constraint.lhs.path();
  const FeaturePath &rhs_path = constraint.rhs.path();

  boost::shared_ptr<const FeatureStructure> lhs;
  if (lhs_path.empty()) {
    lhs = lhs_root;
  } else {
    lhs = lhs_root->Get(lhs_path.begin(), lhs_path.end());
    if (!lhs) {
      // Full evaluation will create an empty value here and then unification
      // is guaranteed to succeed.
      return true;
    }
  }

  boost::shared_ptr<const FeatureStructure> rhs;
  if (rhs_path.empty()) {
    rhs = rhs_root;
  } else {
    rhs = rhs_root->Get(rhs_path.begin(), rhs_path.end());
    if (!rhs) {
      // Full evaluation will create an empty value here and then unification
      // is guaranteed to succeed.
      return true;
    }
  }
  return lhs->PossiblyUnifiable(rhs);
}

Interpretation::Interpretation(int index,
                               boost::shared_ptr<FeatureStructure> fs)
    : probability_(1.0f) {
  values_.insert(std::make_pair(index, fs));
}

Interpretation::Interpretation(const PotentialInterpretation &pi)
    : probability_(pi.prev().probability_) {
  // Populate values_ by first cloning the feature structures from the
  // previous interpretation.  MultiClone is used to ensure that if a value is
  // shared between feature structures in the previous interpretation then the
  // cloned value is also shared between feature structures in values_.  This
  // is cheaper, first, because it requires less cloning, and second, in the
  // context of subsequent constraint evaluation, because unifying
  // previously-unified values requires virtually no work.
  const Interpretation::Map &prev_values = pi.prev().values_;
  FeatureStructure::MultiClone(MappedValueIterator(prev_values.begin()),
                               MappedValueIterator(prev_values.end()),
                               MappedValueInserter(prev_values, values_));
  // Make a clone of the new feature structure for insertion / merging.
  boost::shared_ptr<FeatureStructure> clone = pi.fs()->Clone();
  // If the previous interpretation already contained a feature structure
  // at the new index then attempt to unify the old and new FS.  Otherwise
  // insert the clone.
  Map::iterator p = values_.find(pi.index());
  if (p == values_.end()) {
    values_.insert(std::make_pair(pi.index(), clone));
  } else {
    boost::shared_ptr<FeatureStructure> prev_fs = values_[pi.index()];
    bool success = FeatureStructure::Unify(prev_fs, clone);
    if (!success) {
      values_.clear();
    }
  }
}

boost::shared_ptr<FeatureStructure> Interpretation::GetFS(int index) {
  Map::iterator p = values_.find(index);
  return (p == values_.end()) ? boost::shared_ptr<FeatureStructure>()
         : p->second;
}

boost::shared_ptr<const FeatureStructure> Interpretation::GetFS(
    int index) const {
  Map::const_iterator p = values_.find(index);
  return (p == values_.end()) ? boost::shared_ptr<const FeatureStructure>()
         : p->second;
}

bool Interpretation::Eval(const ConstraintSet &cs) {
  // Reset probability_ since we'll recalculate it in full.
  probability_ = 1.0f;

  for (RelConstraintSet::ConstIterator p = cs.rel_set().Begin();
       p != cs.rel_set().End(); ++p) {
    const RelConstraint &constraint = **p;
    if (!Eval(constraint)) {
      return false;
    }
  }
  for (AbsConstraintSet::ConstIterator p = cs.abs_set().Begin();
       p != cs.abs_set().End(); ++p) {
    const AbsConstraint &constraint = **p;
    if (!Eval(constraint)) {
      return false;
    }
  }
  for (VarConstraintSet::ConstIterator p = cs.var_set().Begin();
       p != cs.var_set().End(); ++p) {
    const VarConstraint &constraint = **p;
    if (!Eval(constraint)) {
      return false;
    }
  }

  return true;
}

bool Interpretation::Eval(const AbsConstraint &constraint) {
  int index = constraint.lhs.index();
  const FeaturePath &path = constraint.lhs.path();
  assert(!path.empty());
  AtomicValue atom = constraint.rhs.value();

  Map::const_iterator p = values_.find(index);
  if (p == values_.end()) {
    return true;  // Passes by default.
  }
  boost::shared_ptr<FeatureStructure> root = p->second;
  boost::shared_ptr<FeatureStructure> val = root->Get(path.begin(), path.end());
  if (!val) {
    root->CreateAtomicValue(path.begin(), path.end(), atom);
    return true;
  }
  return val->IsAtomic() && val->GetAtomicValue() == atom;
}

bool Interpretation::Eval(const RelConstraint &constraint) {
  int lhs_index = constraint.lhs.index();
  int rhs_index = constraint.rhs.index();
  const FeaturePath &lhs_path = constraint.lhs.path();
  const FeaturePath &rhs_path = constraint.rhs.path();

  Map::const_iterator p = values_.find(lhs_index);
  if (p == values_.end()) {
    return true;  // Pass by default.
  }
  Map::const_iterator q = values_.find(rhs_index);
  if (q == values_.end()) {
    return true;  // Pass by default.
  }

  boost::shared_ptr<FeatureStructure> lhs_root = p->second;
  assert(lhs_root);
  boost::shared_ptr<FeatureStructure> rhs_root = q->second;
  assert(rhs_root);

  boost::shared_ptr<FeatureStructure> lhs;
  if (lhs_path.empty()) {
    lhs = lhs_root;
  } else {
    lhs = lhs_root->Get(lhs_path.begin(), lhs_path.end());
    if (!lhs) {
      lhs = lhs_root->CreateEmptyValue(lhs_path.begin(), lhs_path.end());
    }
    assert(lhs);
  }

  boost::shared_ptr<FeatureStructure> rhs;
  if (rhs_path.empty()) {
    rhs = rhs_root;
  } else {
    rhs = rhs_root->Get(rhs_path.begin(), rhs_path.end());
    if (!rhs) {
      rhs = rhs_root->CreateEmptyValue(rhs_path.begin(), rhs_path.end());
    }
    assert(rhs);
  }
  return FeatureStructure::Unify(lhs, rhs);
}

bool Interpretation::Eval(const VarConstraint &constraint) {
  int index = constraint.lhs.index();
  const FeaturePath &path = constraint.lhs.path();
  assert(!path.empty());

  Map::const_iterator p = values_.find(index);
  if (p == values_.end()) {
    // Probability is unchanged if constraint doesn't apply.
    return true;
  }

  boost::shared_ptr<FeatureStructure> root = p->second;
  boost::shared_ptr<FeatureStructure> val = root->Get(path.begin(), path.end());
  if (!val) {
    // There's no value to score: use the most favourable probability.
    probability_ *= constraint.rhs.MaxProbability();
    return true;
  }

  if (!val->IsAtomic()) {
    return false;
  }

  AtomicValue atom = val->GetAtomicValue();
  VarTerm::ProbabilityMap::const_iterator q =
    constraint.rhs.probabilities().find(atom);
  if (q == constraint.rhs.probabilities().end()) {
    // For now at least, interpretation gets zero probability.
    probability_ = 0.0f;
    // FIXME Is this correct for soft constraints?
    return false;
  } else {
    probability_ *= q->second;
    return true;
  }
}

}  // namespace taco
