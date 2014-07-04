#ifndef TACO_SRC_TACO_FEATURE_STRUCTURE_SET_H_
#define TACO_SRC_TACO_FEATURE_STRUCTURE_SET_H_

#include <set>

#include <boost/shared_ptr.hpp>

#include "taco/base/utility.h"
#include "taco/feature_structure.h"

namespace taco {

class FeatureStructureSet {
 public:
  typedef boost::shared_ptr<const FeatureStructure> value_type;
  typedef DereferencingOrderer<value_type, FeatureStructureOrderer> Orderer;
  typedef std::set<value_type, Orderer> SetType;
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
