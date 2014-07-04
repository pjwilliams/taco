#include "vocab_parser.h"

#include "taco/base/exception.h"
#include "taco/base/string_util.h"

#include <istream>
#include <string>

namespace taco {
namespace tool {

VocabParser::VocabParser()
    : input_(0) {
}

VocabParser::VocabParser(std::istream &input)
    : input_(&input) {
  ++(*this);
}

VocabParser &VocabParser::operator++() {
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

void VocabParser::ParseLine(const StringPiece &line) {
  // Symbol.
  std::size_t pos = line.find("|||");
  if (pos == std::string::npos) {
    throw Exception("missing first delimiter");
  }
  value_.symbol = line.substr(0, pos);
  Trim(value_.symbol);

  // Count.
  std::size_t begin = pos+3;
  pos = line.find("|||", begin);
  if (pos == std::string::npos) {
    throw Exception("missing second delimiter");
  }
  value_.count = line.substr(begin, pos-begin);
  Trim(value_.count);

  // POS label set.
  begin = pos+3;
  pos = line.find("|||", begin);
  if (pos == std::string::npos) {
    throw Exception("missing third delimiter");
  }
  StringPiece col = line.substr(begin, pos-begin);
  value_.pos_set.clear();
  Tokenize(value_.pos_set, col);

  // POS counts
  col = line.substr(pos+3);
  value_.pos_counts.clear();
  Tokenize(value_.pos_counts, col);
}

bool operator==(const VocabParser &lhs, const VocabParser &rhs) {
  return lhs.input_ == rhs.input_;
}

bool operator!=(const VocabParser &lhs, const VocabParser &rhs) {
  return !(lhs == rhs);
}

}  // namespace tool
}  // namespace taco
