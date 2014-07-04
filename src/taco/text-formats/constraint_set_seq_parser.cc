#include "taco/text-formats/constraint_set_seq_parser.h"

namespace taco {

ConstraintSetSeqParser::ConstraintSetSeqParser() {}

ConstraintSetSeqParser::ConstraintSetSeqParser(const StringPiece &s,
                                               Vocabulary &feature_set,
                                               Vocabulary &value_set)
    : ConstraintSetParserBase(feature_set, value_set) {
  tokeniser_ = internal::ConstraintTokeniser(s);
  lookahead_ = *tokeniser_;
  if (lookahead_.type == internal::ConstraintToken_EOS) {
    value_.reset();
    // FIXME Match?
    return;
  } else {
    value_ = NTConstraintSet();
  }
}

ConstraintSetSeqParser &ConstraintSetSeqParser::operator++() {
  if (lookahead_.type == internal::ConstraintToken_EOS) {
    value_.reset();
    // FIXME Match?
    return *this;
  }
  Match(internal::ConstraintToken_COLON);
  value_ = NTConstraintSet();
  return *this;
}

ConstraintSetSeqParser ConstraintSetSeqParser::operator++(int) {
  ConstraintSetSeqParser tmp(*this);
  ++(*this);
  return tmp;
}

bool operator==(const ConstraintSetSeqParser &lhs,
                const ConstraintSetSeqParser &rhs) {
  if (!lhs.value_ || !rhs.value_) {
    return !lhs.value_ && !rhs.value_;
  }
  return lhs.tokeniser_ == rhs.tokeniser_;
}

bool operator!=(const ConstraintSetSeqParser &lhs,
                const ConstraintSetSeqParser &rhs) {
  return !(lhs == rhs);
}

} // namespace taco
