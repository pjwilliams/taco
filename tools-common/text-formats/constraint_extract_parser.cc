#include "constraint_extract_parser.h"

#include "taco/base/exception.h"
#include "taco/base/string_util.h"

#include <istream>
#include <string>

namespace taco {
namespace tool {

ConstraintExtractParser::ConstraintExtractParser()
    : input_(0) {
}

ConstraintExtractParser::ConstraintExtractParser(std::istream &input)
    : input_(&input) {
  ++(*this);
}

ConstraintExtractParser &ConstraintExtractParser::operator++() {
  if (!input_) {
    return *this;
  }
  if (!std::getline(*input_, value_.tmp_line)) {
    input_ = 0;
    return *this;
  }
  // Optimisation for sorted extract files.
  // TODO Add option to disable this in case file is unsorted?
  value_.is_identical_to_previous = (value_.tmp_line == value_.line);
  if (!value_.is_identical_to_previous) {
    value_.line.swap(value_.tmp_line);
    ParseLine(value_.line);
  }
  return *this;
}

void ConstraintExtractParser::ParseLine(const StringPiece &line) {
  // LHS.
  size_t pos = line.find("|||");
  if (pos == std::string::npos) {
    throw Exception("missing first delimiter");
  }
  value_.lhs = line.substr(0, pos);
  Trim(value_.lhs);

  // RHS.
  size_t begin = pos+3;
  pos = line.find("|||", begin);
  if (pos == std::string::npos) {
    throw Exception("missing second delimiter");
  }
  StringPiece col = line.substr(begin, pos-begin);
  value_.rhs.clear();
  Tokenize(value_.rhs, col);

  // Constraint sets.
  value_.constraint_sets = line.substr(pos+3);
  Trim(value_.constraint_sets);
}

bool operator==(const ConstraintExtractParser &lhs,
                const ConstraintExtractParser &rhs) {
  // TODO Is this right?  Compare values of istreams if non-zero?
  return lhs.input_ == rhs.input_;
}

bool operator!=(const ConstraintExtractParser &lhs,
                const ConstraintExtractParser &rhs) {
  return !(lhs == rhs);
}

}  // namespace tool
}  // namespace taco
