#include "tools-common/feature_selection_map.h"

#include "taco/base/exception.h"
#include "taco/text-formats/feature_tree_parser.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <istream>
#include <string>

namespace taco {
namespace tool {

void FeatureSelectionMap::insert(const KeyType &key, int index) {
  m_map[key] = index;
}

bool FeatureSelectionMap::lookup(const KeyType &key, int &index) const {
  MapType::const_iterator p = m_map.find(key);
  if (p != m_map.end()) {
    index = p->second;
    return true;
  }
  KeyType backoffKey;
  backoffKey.first = key.first;
  p = m_map.find(backoffKey);
  if (p != m_map.end()) {
    index = p->second;
    return true;
  }
  return false;
}

FeatureSelectionMapParser::FeatureSelectionMapParser()
    : m_input(0) {
}

FeatureSelectionMapParser::FeatureSelectionMapParser(std::istream &input)
    : m_input(&input) {
  ++(*this);
}

FeatureSelectionMapParser &FeatureSelectionMapParser::operator++() {
  if (!m_input) {
    return *this;
  }
  std::string line;
  if (!std::getline(*m_input, line)) {
    m_input = 0;
    return *this;
  }
  parseLine(line);
  return *this;
}

FeatureSelectionMapParser FeatureSelectionMapParser::operator++(int) {
  FeatureSelectionMapParser tmp(*this);
  ++(*this);
  return tmp;
}

void FeatureSelectionMapParser::parseLine(const std::string &line) {
  // Label
  size_t pos = line.find("|||");
  if (pos == std::string::npos) {
    throw Exception("missing delimiter");
  }
  std::string text = line.substr(0, pos);
  boost::trim(text);

  std::vector<std::string> parts;
  boost::split(parts, text, boost::algorithm::is_any_of("-"));

  assert(parts.size() == 1 || parts.size() == 2);
  m_value.label.first = parts[0];
  m_value.label.second = (parts.size() == 2) ? parts[1] : "";

  // Index
  text = line.substr(pos+3);
  boost::trim(text);
  // TODO Error handling
  m_value.index = boost::lexical_cast<int>(text);
}

bool operator==(const FeatureSelectionMapParser &lhs,
                const FeatureSelectionMapParser &rhs) {
  // TODO Is this right?  Compare values of istreams if non-zero?
  return lhs.m_input == rhs.m_input;
}

bool operator!=(const FeatureSelectionMapParser &lhs,
                const FeatureSelectionMapParser &rhs) {
  return !(lhs == rhs);
}

}  // namespace tool
}  // namespace taco
