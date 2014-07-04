#ifndef TACO_SRC_TACO_CONSTRAINT_SET_SET_H_
#define TACO_SRC_TACO_CONSTRAINT_SET_SET_H_

#include <algorithm>
#include <set>

#include <boost/shared_ptr.hpp>

#include "taco/constraint_set.h"
#include "taco/base/utility.h"

namespace taco {

class ConstraintSetSet {
 private:
  typedef DereferencingOrderer<boost::shared_ptr<ConstraintSet>,
          ConstraintSetOrderer> Orderer;
  typedef std::set<boost::shared_ptr<ConstraintSet>, Orderer> Set;

 public:
  typedef Set::iterator iterator;
  typedef Set::const_iterator const_iterator;

  iterator begin() { return m_set.begin(); }
  iterator end() { return m_set.end(); }

  const_iterator begin() const { return m_set.begin(); }
  const_iterator end() const { return m_set.end(); }

  bool empty() const { return m_set.empty(); }
  size_t size() const { return m_set.size(); }

  void clear() { m_set.clear(); }

  std::pair<iterator, bool> insert(boost::shared_ptr<ConstraintSet> c) {
    return m_set.insert(c);
  }

  iterator find(const boost::shared_ptr<ConstraintSet> & key) const {
    return m_set.find(key);
  }

 private:
  Set m_set;
};

class ConstraintSetSetOrderer {
 public:
  bool operator()(const ConstraintSetSet &a,
                  const ConstraintSetSet &b) const {
    typedef DereferencingOrderer<boost::shared_ptr<ConstraintSet>,
                                 ConstraintSetOrderer
                                > Orderer;
    return std::lexicographical_compare(a.begin(), a.end(),
                                        b.begin(), b.end(), Orderer());
  }
};

}  // namespace taco

#endif
