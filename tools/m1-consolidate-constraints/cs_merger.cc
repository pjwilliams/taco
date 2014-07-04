#include "cs_merger.h"

namespace taco {
namespace tool {
namespace m1 {

CSMerger::CSMerger(Vocabulary &feature_set) {
  case_path_.push_back(feature_set.Insert("INFL"));
  case_path_.push_back(feature_set.Insert("CASE"));
}

bool CSMerger::Merge(const ConstraintSet &a,
                     const ConstraintSet &b,
                     boost::shared_ptr<ConstraintSet> &c) {
  // Compare all non-case constraints.  If any differ then the merge fails.
  if (!CompareNonCaseConstraints(a, b)) {
    return false;
  }
  // Find the case constraints.  Each ConstraintSet should have 0 or 1 of them.
  // An exception will be thrown if there's more than one.
  ConstraintType a_cc_type;
  ConstraintType b_cc_type;
  const Constraint *a_cc = FindCaseConstraint(a, a_cc_type);
  const Constraint *b_cc = FindCaseConstraint(b, b_cc_type);
  if (!a_cc && !b_cc) {
    // If neither has a case constraint then the two sets must be identical.
    // The merged result is a copy of either one.
    c.reset(new ConstraintSet(a));
    return true;
  } else if (a_cc && b_cc) {
    // If both have case constraints then the merge contains a VarConstraint
    // that includes all possible values from both.  If all four case values
    // possible then the VarConstraint is dropped.
    boost::shared_ptr<VarConstraint> vc;
    vc = MergeCaseConstraints(*a_cc, a_cc_type, *b_cc, b_cc_type);
    c = CopyNonCaseConstraints(a);
    if (vc.get()) {
      c->var_set().Insert(vc);
    }
    return true;
  } else {
    // One set has a case constraint and the other doesn't.  The case
    // constraint is dropped.
    c = CopyNonCaseConstraints(a);
    return true;
  }
}

bool CSMerger::IsCaseConstraint(const AbsConstraint &ac) {
  return ac.lhs.path() == case_path_;
}

bool CSMerger::IsCaseConstraint(const VarConstraint &vc) {
  return vc.lhs.path() == case_path_;
}

bool CSMerger::CompareNonCaseConstraints(const ConstraintSet &a,
                                         const ConstraintSet &b) {
  // Compare AbsConstraints.
  AbsConstraintEqualityPred abs_equal;
  AbsConstraintSet::ConstIterator q = b.abs_set().Begin();
  for (AbsConstraintSet::ConstIterator p = a.abs_set().Begin();
       p != a.abs_set().End(); ++p) {
    const boost::shared_ptr<const AbsConstraint> &pc = *p;
    if (IsCaseConstraint(*pc)) {
      continue;
    }
    while (IsCaseConstraint(**q)) {
      ++q;
      if (q == b.abs_set().End()) {
        return false;
      }
    }
    if (!abs_equal(*pc, **q)) {
      return false;
    }
    q++;
  }
  while (q != b.abs_set().End()) {
    if (!IsCaseConstraint(**q)) {
      return false;
    }
    q++;
  }
  // Compare RelConstraints.
  RelConstraintSetEqualityPred rel_set_equal;
  if (!rel_set_equal(a.rel_set(), b.rel_set())) {
    return false;
  }
  // Compare VarConstraints.
  {
    VarConstraintEqualityPred var_equal;
    VarConstraintSet::ConstIterator q = b.var_set().Begin();
    for (VarConstraintSet::ConstIterator p = a.var_set().Begin();
         p != a.var_set().End(); ++p) {
      const boost::shared_ptr<const VarConstraint> &pc = *p;
      if (IsCaseConstraint(*pc)) {
        continue;
      }
      while (IsCaseConstraint(**q)) {
        ++q;
        if (q == b.var_set().End()) {
          return false;
        }
      }
      if (!var_equal(*pc, **q)) {
        return false;
      }
      q++;
    }
    while (q != b.var_set().End()) {
      if (!IsCaseConstraint(**q)) {
        return false;
      }
      q++;
    }
  }
  return true;
}

const Constraint *CSMerger::FindCaseConstraint(const ConstraintSet &cs,
                                               ConstraintType &type) {
  // TODO Check for spurious extra case constraints and throw exception.
  for (AbsConstraintSet::ConstIterator p = cs.abs_set().Begin();
       p != cs.abs_set().End(); ++p) {
    const boost::shared_ptr<const AbsConstraint> &ac = *p;
    if (IsCaseConstraint(*ac)) {
      type = kAbsConstraint;
      return ac.get();
    }
  }
  for (VarConstraintSet::ConstIterator p = cs.var_set().Begin();
       p != cs.var_set().End(); ++p) {
    const boost::shared_ptr<const VarConstraint> &vc = *p;
    if (IsCaseConstraint(*vc)) {
      type = kVarConstraint;
      return vc.get();
    }
  }
  return 0;
}

boost::shared_ptr<VarConstraint> CSMerger::MergeCaseConstraints(
      const Constraint &c1, ConstraintType type1,
      const Constraint &c2, ConstraintType type2) {
  VarTerm::ProbabilityMap prob_map;
  int c1_index;
  int c2_index;
  if (type1 == kAbsConstraint) {
    const AbsConstraint *ac = static_cast<const AbsConstraint*>(&c1);
    prob_map[ac->rhs.value()] = 1;
    c1_index = ac->lhs.index();
  } else if (type1 == kVarConstraint) {
    const VarConstraint *vc = static_cast<const VarConstraint*>(&c1);
    prob_map = vc->rhs.probabilities();
    c1_index = vc->lhs.index();
  }

  if (type2 == kAbsConstraint) {
    const AbsConstraint *ac = static_cast<const AbsConstraint*>(&c2);
    prob_map[ac->rhs.value()] = 1;
    c2_index = ac->lhs.index();
  } else if (type2 == kVarConstraint) {
    const VarConstraint *vc = static_cast<const VarConstraint*>(&c2);
    prob_map.insert(vc->rhs.probabilities().begin(),
                    vc->rhs.probabilities().end());
    c2_index = vc->lhs.index();
  }

  // TODO Warn if c1_index and c2_index are different?

  boost::shared_ptr<VarConstraint> result;
  if (prob_map.size() < 4) {
    VarTerm rhs(prob_map);
    PathTerm lhs(c1_index, case_path_);
    result.reset(new VarConstraint(lhs, rhs));
  }
  return result;
}

boost::shared_ptr<ConstraintSet> CSMerger::CopyNonCaseConstraints(
      const ConstraintSet &cs) {
  // Create an empty constraint set.
  boost::shared_ptr<ConstraintSet> cs2(new ConstraintSet());
  // Copy in all AbsConstraints except case constraints.
  cs2->abs_set().Reserve(cs.abs_set().Size());
  for (AbsConstraintSet::ConstIterator p = cs.abs_set().Begin();
       p != cs.abs_set().End(); ++p) {
    const boost::shared_ptr<const AbsConstraint> &ac = *p;
    if (!IsCaseConstraint(*ac)) {
      cs2->abs_set().Insert(ac);
    }
  }
  // Copy in all RelConstraints.
  cs2->rel_set() = cs.rel_set();
  // Copy in all VarConstraints except case constraints.
  cs2->var_set().Reserve(cs.var_set().Size());
  for (VarConstraintSet::ConstIterator p = cs.var_set().Begin();
       p != cs.var_set().End(); ++p) {
    const boost::shared_ptr<const VarConstraint> &vc = *p;
    if (!IsCaseConstraint(*vc)) {
      cs2->var_set().Insert(vc);
    }
  }
  return cs2;
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
