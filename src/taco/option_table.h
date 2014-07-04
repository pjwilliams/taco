#ifndef TACO_SRC_TACO_OPTION_TABLE_H_
#define TACO_SRC_TACO_OPTION_TABLE_H_

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "taco/feature_structure.h"

namespace taco {

class OptionColumn {
 public:
  typedef std::vector<boost::shared_ptr<const FeatureStructure> > ColType;
  typedef ColType::iterator iterator;
  typedef ColType::const_iterator const_iterator;

  OptionColumn() {}
  OptionColumn(const ColType &col) : col_(col) {}

  iterator begin() { return col_.begin(); }
  iterator end() { return col_.end(); }

  const_iterator begin() const { return col_.begin(); }
  const_iterator end() const { return col_.end(); }

  bool IsEmpty() const { return col_.empty(); }
  size_t Size() const { return col_.size(); }

  void Add(boost::shared_ptr<FeatureStructure>);

  void Clear() { col_.clear(); }

  template<typename InputIterator>
  void Assign(InputIterator, InputIterator);

 private:
  ColType col_;
};

class OptionTable {
 private:
  typedef std::map<size_t, OptionColumn> TableType;
 public:
  typedef TableType::iterator iterator;
  typedef TableType::const_iterator const_iterator;

  iterator begin() { return table_.begin(); }
  iterator end() { return table_.end(); }

  const_iterator begin() const { return table_.begin(); }
  const_iterator end() const { return table_.end(); }

  bool IsEmpty() const { return table_.empty(); }
  size_t Size() const { return table_.size(); }

  void Clear() { table_.clear(); }

  // Set the feature structure options for a single rule element.
  void AddColumn(size_t, const OptionColumn &);

  template<typename InputIterator>
  void AddColumn(size_t, InputIterator, InputIterator);

  // Add a column containing a single empty feature structure.
  void AddWildcardColumn(size_t);

  // Add a column containing no feature structures.
  // TODO What is this used for?  Doesn't it guarantee constraint failure?
  void AddEmptyColumn(size_t);

  void DeleteColumn(size_t);

 private:
  TableType table_;
};

template<typename InputIterator>
void OptionColumn::Assign(InputIterator first, InputIterator last) {
  col_.assign(first, last);
}

template<typename InputIterator>
void OptionTable::AddColumn(size_t index, InputIterator first,
                            InputIterator last) {
  OptionColumn &col = table_[index];
  col.Clear();
  col.Assign(first, last);
}

}  // namespace taco

#endif
