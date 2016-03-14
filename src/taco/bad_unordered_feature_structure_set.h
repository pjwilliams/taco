#ifndef TACO_SRC_TACO_BAD_UNORDERED_FEATURE_STRUCTURE_SET_H_
#define TACO_SRC_TACO_BAD_UNORDERED_FEATURE_STRUCTURE_SET_H_

#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>

#include "taco/base/utility.h"
#include "taco/feature_structure.h"

namespace taco {

class BadUnorderedFeatureStructureSet {
 public:
  typedef boost::shared_ptr<const FeatureStructure> value_type;
  typedef DereferencingHasher<value_type, BadFeatureStructureHasher> Hash;
  typedef DereferencingOrderer<value_type, BadFeatureStructureEqualityPred> Equal;
  typedef boost::unordered_set<value_type, Hash, Equal> SetType;
  typedef SetType::iterator iterator;
  typedef SetType::const_iterator const_iterator;

  iterator begin() { return m_set.begin(); }
  iterator end() { return m_set.end(); }

  const_iterator begin() const { return m_set.begin(); }
  const_iterator end() const { return m_set.end(); }

  bool empty() const { return m_set.empty(); }
  size_t size() const { return m_set.size(); }

  std::pair<iterator, bool> insert (const value_type &x) {
    return m_set.insert(x);
  }

  void clear() { m_set.clear(); }

 private:
  SetType m_set;
};

}  // namespace taco

#endif
