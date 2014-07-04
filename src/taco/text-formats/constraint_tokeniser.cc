#include "taco/text-formats/constraint_tokeniser.h"

#include <cassert>
#include <cctype>

namespace taco {
namespace internal {

const std::string &GetConstraintTokenTypeName(ConstraintTokenType type) {
  static std::map<ConstraintTokenType, std::string> names;
  static std::string unknown = "???";
  if (names.empty()) {
    names[ConstraintToken_COLON]        = "COLON";
    names[ConstraintToken_COMMA]        = "COMMA";
    names[ConstraintToken_DIGITSEQ]     = "DIGITSEQ";
    names[ConstraintToken_EOS]          = "EOS";
    names[ConstraintToken_EQUALS]       = "EQUALS";
    names[ConstraintToken_LANGLE]       = "LANGLE";
    names[ConstraintToken_LCURLY]       = "LCURLY";
    names[ConstraintToken_PROBABILITY]  = "PROBABILITY";
    names[ConstraintToken_RANGLE]       = "RANGLE";
    names[ConstraintToken_RCURLY]       = "RCURLY";
    names[ConstraintToken_STRING]       = "STRING";
    names[ConstraintToken_VARNAME]      = "VARNAME";
  }
  std::map<ConstraintTokenType, std::string>::const_iterator p =
    names.find(type);
  return p == names.end() ? unknown : p->second;
}

ConstraintToken::ConstraintToken(ConstraintTokenType t, const std::string &v,
                                 size_t p)
    : type(t)
    , value(v)
    , pos(p) {
}

ConstraintTokeniser::ConstraintTokeniser()
    : value_(ConstraintToken_EOS, "", -1) {
}

ConstraintTokeniser::ConstraintTokeniser(const StringPiece &s)
    : str_(s)
    , value_(ConstraintToken_EOS, "", -1)
    , iter_(s.begin())
    , end_(s.end())
    , pos_(0) {
  ++(*this);
}

ConstraintTokeniser &ConstraintTokeniser::operator++() {
  while (iter_ != end_ && (*iter_ == ' ' || *iter_ == '\t')) {
    ++iter_;
    ++pos_;
  }

  if (iter_ == end_) {
    value_.type = ConstraintToken_EOS;
    value_.value.clear();
    value_.pos = pos_;
    return *this;
  }

  if (*iter_ == ':') {
    value_.type = ConstraintToken_COLON;
    value_.value = ":";
    value_.pos = pos_;
    ++iter_;
    ++pos_;
  } else if (*iter_ == ',') {
    value_.type = ConstraintToken_COMMA;
    value_.value = ",";
    value_.pos = pos_;
    ++iter_;
    ++pos_;
  } else if (*iter_ == '=') {
    value_.type = ConstraintToken_EQUALS;
    value_.value = "=";
    value_.pos = pos_;
    ++iter_;
    ++pos_;
  } else if (*iter_ == '<') {
    value_.type = ConstraintToken_LANGLE;
    value_.value = "<";
    value_.pos = pos_;
    ++iter_;
    ++pos_;
  } else if (*iter_ == '{') {
    value_.type = ConstraintToken_LCURLY;
    value_.value = "{";
    value_.pos = pos_;
    ++iter_;
    ++pos_;
  } else if (*iter_ == '>') {
    value_.type = ConstraintToken_RANGLE;
    value_.value = ">";
    value_.pos = pos_;
    ++iter_;
    ++pos_;
  } else if (*iter_ == '}') {
    value_.type = ConstraintToken_RCURLY;
    value_.value = "}";
    value_.pos = pos_;
    ++iter_;
    ++pos_;
  } else if (*iter_ == '"') {
    value_.type = ConstraintToken_STRING;
    value_.value.clear();
    value_.pos = pos_;
    while (true) {
      ++iter_;
      ++pos_;
      if (iter_ != end_ && *iter_ == '"') {
        break;
      }
      if (iter_ != end_ && *iter_ == '\\') {
        ++iter_;
        ++pos_;
      }
      if (iter_ == end_) {
        // TODO throw exception
        assert(false);
      }
      value_.value += *iter_;
    }
    ++iter_;
    ++pos_;
  } else if (std::isdigit(*iter_)) {
    size_t start = pos_;
    do {
      ++iter_;
      ++pos_;
    } while (iter_ != end_ && std::isdigit(*iter_));
    if (iter_ != end_ && *iter_ == '.') {
      do {
        ++iter_;
        ++pos_;
      } while (iter_ != end_ && std::isdigit(*iter_));
      StringPiece seq = str_.substr(start, pos_-start);
      value_ = ConstraintToken(ConstraintToken_PROBABILITY, seq.as_string(),
                                start);
    } else {
      StringPiece seq = str_.substr(start, pos_-start);
      value_ = ConstraintToken(ConstraintToken_DIGITSEQ, seq.as_string(),
                                start);
    }
  } else if (std::isalpha(*iter_)) {
    size_t start = pos_;
    do {
      ++iter_;
      ++pos_;
    } while (iter_ != end_ && std::isalnum(*iter_));
    StringPiece name = str_.substr(start, pos_-start);
    value_.type = ConstraintToken_VARNAME;
    value_.value = name.as_string();
    value_.pos = start;
  } else {
    // TODO throw exception
    assert(false);
  }

  return *this;
}

ConstraintTokeniser ConstraintTokeniser::operator++(int) {
  ConstraintTokeniser tmp(*this);
  ++*this;
  return tmp;
}

bool operator==(const ConstraintTokeniser &lhs,
                const ConstraintTokeniser &rhs) {
  if (lhs.value_.type == ConstraintToken_EOS ||
      rhs.value_.type == ConstraintToken_EOS) {
    return lhs.value_.type == ConstraintToken_EOS &&
           rhs.value_.type == ConstraintToken_EOS;
  }
  return lhs.iter_ == rhs.iter_;
}

bool operator!=(const ConstraintTokeniser &lhs,
                const ConstraintTokeniser &rhs) {
  return !(lhs == rhs);
}

} // namespace internal
} // namespace taco
