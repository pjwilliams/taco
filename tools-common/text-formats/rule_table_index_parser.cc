#include "tools-common/text-formats/rule_table_index_parser.h"

#include "taco/base/exception.h"
#include "taco/base/string_util.h"

#include <boost/algorithm/string.hpp>

#include <istream>
#include <string>

namespace taco {
namespace tool {

RuleTableIndexParser::RuleTableIndexParser(const RuleTableIndexParser &other) {
  input_ = other.input_;
  value_.line = other.value_.line;
  if (!value_.line.empty()) {
    ParseLine(value_.line);
  }
}

RuleTableIndexParser &RuleTableIndexParser::operator=(
    const RuleTableIndexParser &other) {
  input_ = other.input_;
  value_.line = other.value_.line;
  if (!value_.line.empty()) {
    ParseLine(value_.line);
  }
  return *this;
}

RuleTableIndexParser::RuleTableIndexParser()
    : input_(0) {
}

RuleTableIndexParser::RuleTableIndexParser(std::istream &input)
    : input_(&input) {
  ++(*this);
}

RuleTableIndexParser &RuleTableIndexParser::operator++() {
  if (!input_) {
    return *this;
  }
  if (!std::getline(*input_, value_.line)) {
    input_ = 0;
    return *this;
  }
  ParseLine(value_.line);
  return *this;
}

void RuleTableIndexParser::ParseLine(StringPiece line) {
  // Key
  size_t pos = line.find("|||");
  if (pos == std::string::npos) {
    throw Exception("missing delimiter");
  }
  value_.key = line.substr(0, pos);
  Trim(value_.key);

  // Line number
  StringPiece col = line.substr(pos+3);
  value_.line_num = std::atoi(col.as_string().c_str());
}

bool operator==(const RuleTableIndexParser &lhs,
                const RuleTableIndexParser &rhs) {
  return lhs.input_ == rhs.input_;
}

bool operator!=(const RuleTableIndexParser &lhs,
                const RuleTableIndexParser &rhs) {
  return !(lhs == rhs);
}

}  // namespace tool
}  // namespace taco
