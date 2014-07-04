#ifndef TACO_TOOLS_M1_CONSOLIDATE_CONSTRAINTS_CS_MERGER_H_
#define TACO_TOOLS_M1_CONSOLIDATE_CONSTRAINTS_CS_MERGER_H_

#include "taco/constraint.h"
#include "taco/constraint_set.h"
#include "taco/feature_path.h"
#include "taco/base/vocabulary.h"

#include <boost/shared_ptr.hpp>

namespace taco {
namespace tool {
namespace m1 {

class CSMerger {
 public:
  CSMerger(Vocabulary &);

  bool Merge(const ConstraintSet &, const ConstraintSet &,
             boost::shared_ptr<ConstraintSet> &);
 private:
  bool IsCaseConstraint(const AbsConstraint &);

  bool IsCaseConstraint(const VarConstraint &);

  // Test if two constraint sets are equal ignoring case constraints.
  bool CompareNonCaseConstraints(const ConstraintSet &, const ConstraintSet &);

  // Search a constraint set for a case constraint.  Return a pointer to it
  // if found or an empty pointer otherwise.  If there is more than one case
  // constraint then thrown an exception.
  const Constraint *FindCaseConstraint(const ConstraintSet &,
                                       ConstraintType &);

  // Merge a pair of case constraints to produce a VarConstraint that
  // contains all possible values.  If all four case values are possible
  // then return an empty pointer.
  boost::shared_ptr<VarConstraint> MergeCaseConstraints(
      const Constraint &, ConstraintType, const Constraint &, ConstraintType);

  // Copy a ConstraintSet ignoring case constraints.
  boost::shared_ptr<ConstraintSet> CopyNonCaseConstraints(
      const ConstraintSet &);

  FeaturePath case_path_;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
