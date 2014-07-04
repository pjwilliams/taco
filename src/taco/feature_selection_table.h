#ifndef TACO_SRC_TACO_FEATURE_SELECTION_TABLE_H_
#define TACO_SRC_TACO_FEATURE_SELECTION_TABLE_H_

#include <istream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "taco/feature_selection_rule.h"
#include "taco/text-formats/feature_tree_parser.h"

namespace taco {

typedef std::vector<boost::shared_ptr<FeatureSelectionRule>
                   > FeatureSelectionTable;

// Parses the table format used for the feature selection rule file.  Entries
// have a zero-based index and a rule, which is either 'assign', 'drop', or a
// feature tree in bracket notation.  For example,
//
//  0 ||| assign
//  1 ||| drop
//  2 ||| [AGREEMENT:[NUMBER]]
//
class FeatureSelectionTableParser {
 public:
  struct Entry {
    int index;
    boost::shared_ptr<FeatureSelectionRule> rule;
  };

  FeatureSelectionTableParser();
  FeatureSelectionTableParser(std::istream &, Vocabulary &);
  ~FeatureSelectionTableParser();

  const Entry &operator*() const { return value_; }
  const Entry *operator->() const { return &value_; }

  FeatureSelectionTableParser &operator++();
  FeatureSelectionTableParser operator++(int);

  friend bool operator==(const FeatureSelectionTableParser &,
                         const FeatureSelectionTableParser &);
  friend bool operator!=(const FeatureSelectionTableParser &,
                         const FeatureSelectionTableParser &);

 private:
  Entry value_;
  std::istream *input_;
  Vocabulary *feature_set_;
  FeatureTreeParser *parser_;

  void ParseLine(const std::string &);
};

}  // namespace taco

#endif
