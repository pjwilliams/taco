#ifndef TACO_TOOLS_COMMON_FEATURE_SELECTION_MAP_H_
#define TACO_TOOLS_COMMON_FEATURE_SELECTION_MAP_H_

#include "taco/text-formats/feature_tree_parser.h"

#include <boost/shared_ptr.hpp>

#include <istream>
#include <string>
#include <vector>

namespace taco {
namespace tool {

class FeatureSelectionMap {
 private:
  typedef std::pair<std::string, std::string> KeyType;
  typedef std::map<KeyType, int> MapType;

 public:
  void insert(const KeyType &, int);
  bool lookup(const KeyType &, int &) const;

 private:
  MapType m_map;
};

// Parses the table format used for the feature selection map file.  Entries
// have a one- or two-part label and an index.  For example,
//
//  ADJA ||| 0
//  NP-SB ||| 2
//
class FeatureSelectionMapParser {
 public:
  struct Entry {
    std::pair<std::string, std::string> label;
    int index;
  };

  FeatureSelectionMapParser();
  FeatureSelectionMapParser(std::istream &);

  const Entry &operator*() const { return m_value; }
  const Entry *operator->() const { return &m_value; }

  FeatureSelectionMapParser &operator++();
  FeatureSelectionMapParser operator++(int);

  friend bool operator==(const FeatureSelectionMapParser &,
                         const FeatureSelectionMapParser &);
  friend bool operator!=(const FeatureSelectionMapParser &,
                         const FeatureSelectionMapParser &);

private:
  Entry m_value;
  std::istream *m_input;

  void parseLine(const std::string &);
};

}  // namespace tool
}  // namespace taco

#endif
