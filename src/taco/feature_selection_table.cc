#include "taco/feature_selection_table.h"

#include <istream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "taco/base/exception.h"
#include "taco/text-formats/feature_tree_parser.h"

namespace taco {

FeatureSelectionTableParser::FeatureSelectionTableParser()
    : input_(0)
    , feature_set_(0)
    , parser_(0) {
}

FeatureSelectionTableParser::FeatureSelectionTableParser(std::istream &input,
    Vocabulary &feature_set)
    : input_(&input)
    , feature_set_(&feature_set)
    , parser_(new FeatureTreeParser(feature_set)) {
  ++(*this);
}

FeatureSelectionTableParser::~FeatureSelectionTableParser() {
  delete parser_;
}

FeatureSelectionTableParser &FeatureSelectionTableParser::operator++() {
  if (!input_) {
    return *this;
  }
  std::string line;
  if (!std::getline(*input_, line)) {
    input_ = 0;
    return *this;
  }
  ParseLine(line);
  return *this;
}

FeatureSelectionTableParser FeatureSelectionTableParser::operator++(int) {
  FeatureSelectionTableParser tmp(*this);
  ++(*this);
  return tmp;
}

void FeatureSelectionTableParser::ParseLine(const std::string &line) {
  // Index
  size_t pos = line.find("|||");
  if (pos == std::string::npos) {
    throw Exception("missing delimiter");
  }
  std::string text = line.substr(0, pos);
  boost::trim(text);
  // TODO Error handling
  value_.index = boost::lexical_cast<int>(text);

  // Feature selection rule
  text = line.substr(pos+3);
  boost::trim(text);
  if (text == "assign") {
    value_.rule.reset(new FeatureSelectionRule(
                            FeatureSelectionRule::Rule_Assign));
  } else if (text == "drop") {
    value_.rule.reset(new FeatureSelectionRule(
                            FeatureSelectionRule::Rule_Drop));
  } else {
    boost::shared_ptr<FeatureTree> tree = parser_->parse(text);
    value_.rule.reset(new FeatureSelectionRule(
                            FeatureSelectionRule::Rule_Select, tree));
  }
}

bool operator==(const FeatureSelectionTableParser &lhs,
                const FeatureSelectionTableParser &rhs) {
  // TODO Is this right?  Compare values of istreams if non-zero?
  return lhs.input_ == rhs.input_;
}

bool operator!=(const FeatureSelectionTableParser &lhs,
                const FeatureSelectionTableParser &rhs) {
  return !(lhs == rhs);
}

}  // namespace taco
