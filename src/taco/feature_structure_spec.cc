#include "taco/feature_structure_spec.h"

#include <cassert>
#include <memory>
#include <sstream>
#include <utility>

namespace taco {

bool FeatureStructureSpec::IsAtomic() const {
  return content_pairs.size() == 1 &&
         content_pairs.begin()->first.empty() &&
         content_pairs.begin()->second != kNullAtom;
}

bool FeatureStructureSpec::IsComplex() const {
  return !IsAtomic();
}

void FeatureStructureSpec::Nest(const FeaturePath &prefix,
                                const FeatureStructureSpec &spec) {
  assert(!prefix.empty());

  ContentPairSet::const_iterator p;
  ContentPairSet::const_iterator end = spec.content_pairs.end();
  for (p = spec.content_pairs.begin(); p != end; ++p) {
    const FeaturePath &suffix = p->first;
    AtomicValue atom = p->second;

    ContentPair new_pair;
    FeaturePath &path = new_pair.first;
    path.reserve(prefix.size() + suffix.size());
    path.insert(path.end(), prefix.begin(), prefix.end());
    path.insert(path.end(), suffix.begin(), suffix.end());
    new_pair.second = atom;
    content_pairs.insert(new_pair);
  }
}

void FeatureStructureSpec::Write(const Vocabulary &feature_set,
                                 const Vocabulary &value_set,
                                 std::ostream &out) const {
  ContentPairSet::const_iterator p;
  ContentPairSet::const_iterator end = content_pairs.end();
  for (p = content_pairs.begin(); p != end; ++p) {
    const FeaturePath &path = p->first;
    AtomicValue atom = p->second;
    out << "<";
    for (FeaturePath::const_iterator q = path.begin(); q != path.end(); ++q) {
      if (q != path.begin())
        out << ",";
      out << feature_set.Lookup(*q);
    }
    out << "> = ";
    out << value_set.Lookup(atom);
    out << std::endl;
  }
}

}  // namespace taco
