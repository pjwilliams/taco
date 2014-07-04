#ifndef TACO_SRC_TACO_CONSTRAINT_EVALUATOR_H_
#define TACO_SRC_TACO_CONSTRAINT_EVALUATOR_H_

#include <vector>

namespace taco {

class ConstraintSet;
class Interpretation;
class OptionTable;

// TODO 'Recombine' interpretations?  Two interpretations could contain
// feature structures that only differ in values outside the constraints.  It
// might make sense to compress them into a single object, either losslessly
// (by retaining ambiguity) or by discarding all but one.
class ConstraintEvaluator {
 public:
  // Searches for interpretations over the option table that satisfy the
  // constraints.  If the option table is missing a column indexed by the
  // constraints then a 'wildcard' column (containing a single empty feature
  // structure) is inserted prior to evaluation.  Returns true if at least one
  // interpretation is found.
  bool Eval(const OptionTable &, const ConstraintSet &,
            std::vector<Interpretation> &) const;

  // Same as above except that it does not return the set of interpretations.
  // This is potentially faster since Eval only needs to find the first valid
  // interpretation, but currently it's just a wrapper around the first form.
  bool Eval(const OptionTable &, const ConstraintSet &) const;

  // Searches for interpretations that are consistent with both an existing set
  // of interpretations and an option table.  The given interpretations must
  // cover the full set of constraint indices.  Returns true if at least one
  // interpretation is found.
  bool Eval(const std::vector<Interpretation> &, const OptionTable &,
            const ConstraintSet &, std::vector<Interpretation> &) const;
};

}  // namespace taco

#endif
