#include "taco/option_table.h"

namespace taco {

void OptionColumn::Add(boost::shared_ptr<FeatureStructure> fs) {
  col_.push_back(fs);
}

void OptionTable::AddColumn(size_t index, const OptionColumn &col) {
  table_[index] = col;
}

void OptionTable::AddEmptyColumn(size_t index) {
  OptionColumn &col = table_[index];
  col.Clear();
}

void OptionTable::AddWildcardColumn(size_t index) {
  FeatureStructureSpec spec;
  spec.content_pairs.insert(std::make_pair(FeaturePath(), kNullAtom));
  boost::shared_ptr<FeatureStructure> fs(new FeatureStructure(spec));
  OptionColumn &col = table_[index];
  col.Clear();
  col.Add(fs);
}

void OptionTable::DeleteColumn(size_t index) {
  table_.erase(index);
}

}  // namespace taco
