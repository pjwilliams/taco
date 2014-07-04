#ifndef TACO_SRC_TACO_INTERPRETATION_H_
#define TACO_SRC_TACO_INTERPRETATION_H_

#include <map>

#include <boost/container/flat_map.hpp>
#include <boost/shared_ptr.hpp>

#include "taco/constraint.h"
#include "taco/constraint_set.h"
#include "taco/feature_structure.h"

namespace taco {

class Interpretation;

// Stores the information necessary to create a full interpretation and
// provides a QuickCheck operation that (imperfectly) determines whether
// the full interpretation would satisfy a given set of constraints.
//
// Rationale: creating Interpretation objects is expensive since it involves
// cloning all of the input feature structures.  This is wasteful because, at
// least in a typical use case, most interpretations will fail (example:
// interpretation->Eval(constraint_set) fails for approx 80% of interpretations
// when evaluating agreement relations generated for the German portion of the
// WMT11 News Commentary corpus).
//
// A PotentialInterpretation is much cheaper to create since it only contains
// references to the input feature structures.  However, this requires that
// any operations involving the feature structure are non-destructive.
// PotentialInterpretation provides the non-destructive operation QuickCheck(),
// which performs an imperfect check to determine whether the feature structures
// will satisfy a paticular set of constraints.  A full Interpretation can then
// be constructed or not, conditional upon this result.  The check produces
// false positives but never produces false negatives.
//
// As an example of a false positive, consider the extension of an
// interpretation over elements 0, 3, 4, 5 to cover element 6 where the
// constraint set contains the rules <4 AGR> = <6 AGR> and <5 AGR> = <6 AGR>.
// If the AGR values of 4 and 6 are unifiable and the AGR values of 5 and 6
// are also unifiable then QuickCheck() will report success.  Whether this is
// correct or not depends on whether the unified AGR value of 4 and 6 can be
// unified with the AGR value of 5, something that QuickCheck() cannot
// determine.
class PotentialInterpretation {
 public:
  PotentialInterpretation(const Interpretation &prev, int index,
                          boost::shared_ptr<const FeatureStructure> fs)
      : prev_(prev), index_(index), fs_(fs) {}

  bool QuickCheck(const ConstraintSet &);

  const Interpretation &prev() const { return prev_; }
  int index() const { return index_; }
  boost::shared_ptr<const FeatureStructure> fs() const { return fs_; }

 private:
  bool QuickCheck(const AbsConstraint &);
  bool QuickCheck(const RelConstraint &);
  bool QuickCheck(const VarConstraint &);

  const Interpretation &prev_;
  int index_;
  boost::shared_ptr<const FeatureStructure> fs_;
};

// Represents a single interpretation...
class Interpretation {
 private:
  typedef boost::container::flat_map<int,
                                     boost::shared_ptr<FeatureStructure> > Map;

 public:
  typedef Map::iterator iterator;
  typedef Map::const_iterator const_iterator;

  // Creates an Interpretation covering a single rule element.
  Interpretation(int, boost::shared_ptr<FeatureStructure>);

  // Creates an Interpretation from an existing Interpretation plus one new
  // rule element.  If they are incompatible then the resulting interpretation
  // will be empty.
  Interpretation(const PotentialInterpretation &);

  // I'm not sure why, but the assignment operator must be defined when Map is
  // a typedef to boost::container::flat_map (but not std::map).  The generated
  // one (which takes Interpretation & not const Interpretation &) isn't
  // good enough.
  Interpretation &operator=(const Interpretation &other) {
    values_ = other.values_;
    probability_ = other.probability_;
    return *this;
  }

  iterator begin() { return values_.begin(); }
  iterator end() { return values_.end(); }

  const_iterator begin() const { return values_.begin(); }
  const_iterator end() const { return values_.end(); }

  boost::shared_ptr<FeatureStructure> GetFS(int);
  boost::shared_ptr<const FeatureStructure> GetFS(int) const;

  // This will be true if and only if the attempted Interpretation is not valid.
  bool IsEmpty() const { return values_.empty(); }

  float probability() const { return probability_; }

  // Evaluates all constraints that apply to this interpretation, returning
  // true if they all pass and the resulting probability is non-zero.
  bool Eval(const ConstraintSet &);

  // Evaluates a single absolute constraint.  Returns true if the constraint
  // is satisfied.  If the element indexed by the constraint doesn't occur in
  // this interpretation then the constraint is satisifed by default.
  bool Eval(const AbsConstraint &);

  // Evaluates a single relative constraint.  Returns true if the constraint
  // is satisfied.  If either of the elements indexed by the constraint
  // doesn't occur in this interpretation then the constraint is satisifed by
  // default.
  bool Eval(const RelConstraint &);

  // Evaluates a single variable constraint and updates this interpretation's
  // probability.  If the element indexed by the constraint doesn't occur in
  // this interpretation then the constraint probability is unchanged.
  bool Eval(const VarConstraint &);

 private:
  // Forward declare nested classes.
  class MappedValueIterator;
  class MappedValueInserter;

  friend class MappedValueIterator;
  friend class MappedValueInserter;
  friend class PotentialInterpretation;

  Map values_;
  float probability_;
};

}  // namespace taco

#endif
