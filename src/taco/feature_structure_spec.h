#ifndef TACO_SRC_TACO_FEATURE_STRUCTURE_SPEC_H_
#define TACO_SRC_TACO_FEATURE_STRUCTURE_SPEC_H_

#include <set>
#include <utility>

#include "taco/base/basic_types.h"
#include "taco/feature_path.h"

// DEBUG
#include "taco/base/vocabulary.h"
#include <ostream>

namespace taco {

// Represents a specification of a feature structure in terms of path and
// values and path equivalences.
struct FeatureStructureSpec {
 public:
  typedef std::pair<FeaturePath, AtomicValue> ContentPair;
  typedef std::pair<FeaturePath, FeaturePath> EquivPair;
  typedef std::set<ContentPair> ContentPairSet;
  typedef std::set<EquivPair> EquivPairSet;

  bool Empty() const {
    return content_pairs.empty() && equiv_pairs.empty();
  }

  void Clear() { content_pairs.clear(); equiv_pairs.clear(); }

  bool IsAtomic() const;
  bool IsComplex() const;

  // Nests the given FeatureStructureSpec inside this one, by prefixing
  // all of the nested spec's paths with the given prefix.
  void Nest(const FeaturePath &prefix, const FeatureStructureSpec &spec);

  // DEBUG
  void Write(const Vocabulary &, const Vocabulary &, std::ostream &) const;

  ContentPairSet content_pairs;
  EquivPairSet equiv_pairs;

  bool operator<(const FeatureStructureSpec &other) const {
    if (content_pairs == other.content_pairs) {
      return equiv_pairs < other.equiv_pairs;
    }
    return content_pairs < other.content_pairs;
  }
};

}  // namespace taco

#endif
