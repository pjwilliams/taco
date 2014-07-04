#include "taco/text-formats/feature_structure_tokeniser.h"

#include <cctype>

namespace taco {
namespace internal {

FSToken::FSToken(FSTokenType t, StringPiece v, size_t p)
    : type(t)
    , value(v)
    , pos(p) {
}

FSTokeniser::FSTokeniser()
    : value_(FSToken_EOS, "", -1) {
}

FSTokeniser::FSTokeniser(const StringPiece &s)
    : str_(s)
    , value_(FSToken_EOS, "", -1)
    , iter_(s.begin())
    , end_(s.end())
    , pos_(0) {
  ++(*this);
}

FSTokeniser &FSTokeniser::operator++() {
  while (iter_ != end_ && (*iter_ == ' ' || *iter_ == '\t' || *iter_ == ';')) {
    ++iter_;
    ++pos_;
  }

  if (iter_ == end_) {
    value_ = FSToken(FSToken_EOS, "", pos_);
    return *this;
  }

  if (*iter_ == '[') {
    value_ = FSToken(FSToken_LB, "[", pos_);
    ++iter_;
    ++pos_;
  } else if (*iter_ == ']') {
    value_ = FSToken(FSToken_RB, "]", pos_);
    ++iter_;
    ++pos_;
  } else if (*iter_ == ':') {
    value_ = FSToken(FSToken_COLON, ":", pos_);
    ++iter_;
    ++pos_;
  } else {
    size_t start = pos_;
    while (true) {
      ++iter_;
      ++pos_;
      if (iter_ == end_ || *iter_ == ' ' || *iter_ == '\t' || *iter_ == ';') {
        break;
      }
      if (*iter_ == '[' || *iter_ == ']' || *iter_ == ':') {
        break;
      }
    }
    StringPiece word = str_.substr(start, pos_-start);
    value_ = FSToken(FSToken_WORD, word, start);
  }

  return *this;
}

FSTokeniser FSTokeniser::operator++(int) {
  FSTokeniser tmp(*this);
  ++*this;
  return tmp;
}

bool operator==(const FSTokeniser &lhs, const FSTokeniser &rhs) {
  if (lhs.value_.type == FSToken_EOS || rhs.value_.type == FSToken_EOS) {
    return lhs.value_.type == FSToken_EOS && rhs.value_.type == FSToken_EOS;
  }
  return lhs.iter_ == rhs.iter_;
}

bool operator!=(const FSTokeniser &lhs, const FSTokeniser &rhs) {
  return !(lhs == rhs);
}

}  // namespace internal
}  // namespace taco
