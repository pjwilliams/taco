#include "taco/constraint_term.h"

namespace taco {

float VarTerm::MaxProbability() const {
  float max = 0.0f;
  for (ProbabilityMap::const_iterator p = probabilities_.begin();
       p != probabilities_.end(); ++p) {
    float prob = p->second;
    if (prob > max) {
      max = prob;
    }
  }
  return max;
}

bool operator==(const ValueTerm &a, const ValueTerm &b) {
  return a.value() == b.value();
}

bool operator!=(const ValueTerm &a, const ValueTerm &b) {
  return a.value() != b.value();
}

bool operator==(const PathTerm &a, const PathTerm &b) {
  return a.index() == b.index() && a.path() == b.path();
}

bool operator!=(const PathTerm &a, const PathTerm &b) {
  return a.index() != b.index() || a.path() != b.path();
}

bool operator==(const VarTerm &a, const VarTerm &b) {
  return a.probabilities() == b.probabilities();
}

bool operator!=(const VarTerm &a, const VarTerm &b) {
  return a.probabilities() != b.probabilities();
}

bool ValueTermOrderer::operator()(const ValueTerm &a,
                                  const ValueTerm &b) const {
  return a.value() < b.value();
}

bool PathTermOrderer::operator()(const PathTerm &a, const PathTerm &b) const {
  if (a.index() == b.index()) {
    return a.path() < b.path();
  }
  return a.index() < b.index();
}

bool VarTermOrderer::operator()(const VarTerm &a, const VarTerm &b) const {
  return a.probabilities() < b.probabilities();
}

}  // namespace taco
