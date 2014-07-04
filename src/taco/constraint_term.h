#ifndef TACO_SRC_TACO_CONSTRAINT_TERM_H_
#define TACO_SRC_TACO_CONSTRAINT_TERM_H_

#include <map>

#include <boost/container/flat_map.hpp>

#include "taco/base/basic_types.h"
#include "taco/feature_path.h"

namespace taco {

class ValueTerm {
 public:
  ValueTerm(AtomicValue v) : value_(v) {}
  AtomicValue value() const { return value_; }
 private:
  AtomicValue value_;
};

class PathTerm {
 public:
  PathTerm(int i, FeaturePath p) : index_(i), path_(p) {}
  int index() const { return index_; }
  const FeaturePath &path() const { return path_; }
 private:
  int index_;
  FeaturePath path_;
};

class VarTerm {
 public:
  typedef boost::container::flat_map<AtomicValue, float> ProbabilityMap;
  VarTerm(const ProbabilityMap &pm) : probabilities_(pm) {}
  const ProbabilityMap &probabilities() const { return probabilities_; }
  float MaxProbability() const;
 private:
  ProbabilityMap probabilities_;
};

bool operator==(const ValueTerm &, const ValueTerm &);
bool operator!=(const ValueTerm &, const ValueTerm &);

bool operator==(const PathTerm &, const PathTerm &);
bool operator!=(const PathTerm &, const PathTerm &);

bool operator==(const VarTerm &, const VarTerm &);
bool operator!=(const VarTerm &, const VarTerm &);

// Defines a (completely arbitrary) strict weak ordering between PathTerm
// objects.
class PathTermOrderer {
 public:
  bool operator()(const PathTerm &, const PathTerm &) const;
};

// Defines a (completely arbitrary) strict weak ordering between ValueTerm
// objects.
class ValueTermOrderer {
 public:
  bool operator()(const ValueTerm &, const ValueTerm &) const;
};

// Defines a (completely arbitrary) strict weak ordering between VarTerm
// objects.
class VarTermOrderer {
 public:
  bool operator()(const VarTerm &, const VarTerm &) const;
};

}  // namespace taco

#endif
