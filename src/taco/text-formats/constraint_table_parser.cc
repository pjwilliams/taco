#include "taco/text-formats/constraint_table_parser.h"

#include <istream>
#include <string>

#include "taco/base/exception.h"
#include "taco/base/string_util.h"

namespace taco {

ConstraintTableParser::ConstraintTableParser()
    : input_(0) {
}

ConstraintTableParser::ConstraintTableParser(std::istream &input)
    : input_(&input) {
  ++(*this);
}

ConstraintTableParser &ConstraintTableParser::operator++() {
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

ConstraintTableParser ConstraintTableParser::operator++(int) {
  ConstraintTableParser tmp(*this);
  ++(*this);
  return tmp;
}

void ConstraintTableParser::ParseLine(const StringPiece &line) {
  // ID.
  size_t pos = line.find("|||");
  if (pos == std::string::npos) {
    throw Exception("missing first delimiter");
  }
  value_.id = line.substr(0, pos);
  Trim(value_.id);

  // Constraint set.
  value_.constraint_set = line.substr(pos+3);
  Trim(value_.constraint_set);
}

bool operator==(const ConstraintTableParser &lhs,
                const ConstraintTableParser &rhs) {
  return lhs.input_ == rhs.input_;
}

bool operator!=(const ConstraintTableParser &lhs,
                const ConstraintTableParser &rhs) {
  return !(lhs == rhs);
}

}  // namespace taco
