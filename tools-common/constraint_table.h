#ifndef TACO_TOOLS_COMMON_CONSTRAINT_TABLE_H_
#define TACO_TOOLS_COMMON_CONSTRAINT_TABLE_H_

#include "taco/constraint_set_set.h"
#include "taco/base/vocabulary.h"

#include <boost/unordered_map.hpp>

#include <istream>
#include <set>
#include <vector>

namespace taco {
namespace tool {

// Maps the target-side of an SCFG to a sets of constraints.  Note that unlike
// the constraint table file, a ConstraintTable stores the constraint sets and
// not just indices into another data structure.  (TODO A different name might
// help to distinguish the two concepts of 'constraint table.')
class ConstraintTable {
 public:
  typedef std::vector<unsigned int> Key;

 private:
  typedef boost::unordered_map<Key,
                               boost::shared_ptr<ConstraintSetSet> > Table;

 public:
  typedef Table::iterator iterator;
  typedef Table::const_iterator const_iterator;

  iterator begin() { return table_.begin(); }
  iterator end() { return table_.end(); }

  const_iterator begin() const { return table_.begin(); }
  const_iterator end() const { return table_.end(); }

  size_t size() const { return table_.size(); }
  bool empty() const { return table_.empty(); }

  const ConstraintSetSet *Lookup(const Key &) const;

  void Insert(const Key &, boost::shared_ptr<ConstraintSetSet>);

 private:
  Table table_;
};

class BasicConstraintTableLoader {
 public:
  BasicConstraintTableLoader(Vocabulary &/*vocab*/, Vocabulary &feature_set,
                             Vocabulary &value_set)
    : feature_set_(feature_set)
    , value_set_(value_set) {
  }

  void Load(std::istream &, std::istream &, ConstraintTable &) const;

 private:
  Vocabulary &feature_set_;
  Vocabulary &value_set_;
};

}  // namespace tool
}  // namespace taco

#endif
