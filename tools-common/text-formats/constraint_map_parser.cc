#include "constraint_map_parser.h"

#include "taco/base/exception.h"
#include "taco/base/string_util.h"

#include <istream>
#include <string>

namespace taco {
namespace tool {

ConstraintMapParser::ConstraintMapParser(const ConstraintMapParser &other) {
  input_ = other.input_;
  value_.line = other.value_.line;
  if (!value_.line.empty()) {
    ParseLine(value_.line);
  }
}

ConstraintMapParser &ConstraintMapParser::operator=(
    const ConstraintMapParser &other) {
  input_ = other.input_;
  value_.line = other.value_.line;
  if (!value_.line.empty()) {
    ParseLine(value_.line);
  }
  return *this;
}

ConstraintMapParser::ConstraintMapParser()
    : input_(0) {
}

ConstraintMapParser::ConstraintMapParser(std::istream &input)
    : input_(&input) {
  ++(*this);
}

ConstraintMapParser &ConstraintMapParser::operator++() {
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

void ConstraintMapParser::ParseLine(const StringPiece &line) {
  // Key
  size_t pos = line.find("|||");
  if (pos == std::string::npos) {
    throw Exception("missing delimiter");
  }
  value_.key = line.substr(0, pos);
  Trim(value_.key);

  // IDs
  StringPiece col = line.substr(pos+3);
  value_.ids.clear();
  Tokenize(value_.ids, col);
}

bool operator==(const ConstraintMapParser &lhs, const ConstraintMapParser &rhs) {
  return lhs.input_ == rhs.input_;
}

bool operator!=(const ConstraintMapParser &lhs, const ConstraintMapParser &rhs) {
  return !(lhs == rhs);
}

}  // namespace tool
}  // namespace taco
