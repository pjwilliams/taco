#include "taco/text-formats/lexicon_parser.h"

#include <cassert>
#include <sstream>

#include "taco/base/exception.h"
#include "taco/base/string_util.h"

namespace taco {

LexiconParser::LexiconParser()
    : input_(0) {
}

LexiconParser::LexiconParser(std::istream &input)
    : input_(&input) {
  ++(*this);
}

LexiconParser &LexiconParser::operator++() {
  if (!input_) {
    return *this;
  }
  while (true) {
    if (!std::getline(*input_, value_.line)) {
      input_ = 0;
      return *this;
    }
    if (!IsBlankLine(value_.line)) {
      break;
    }
  }
  ParseLine(value_.line);
  return *this;
}

LexiconParser LexiconParser::operator++(int) {
  LexiconParser tmp(*this);
  ++(*this);
  return tmp;
}

bool LexiconParser::IsBlankLine(const std::string &line) {
  std::size_t pos = line.find_first_not_of(" \t");
  return (pos == std::string::npos || line[pos] == '#');
}

void LexiconParser::ParseLine(const StringPiece &line) {
  // Word.
  std::size_t pos = line.find("|||");
  if (pos == std::string::npos) {
    throw Exception("missing delimiter");
  }
  StringPiece s = line.substr(0, pos);
  Trim(s);
  // TODO unescape
  value_.word = s.as_string();

  // FS.
  s = line.substr(pos+3);
  Trim(s);
  value_.fs = s.as_string();
}

bool operator==(const LexiconParser &lhs, const LexiconParser &rhs) {
  return lhs.input_ == rhs.input_;
}

bool operator!=(const LexiconParser &lhs, const LexiconParser &rhs) {
  return !(lhs == rhs);
}

} // namespace taco
