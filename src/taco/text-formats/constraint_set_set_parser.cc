#include "taco/text-formats/constraint_set_set_parser.h"

namespace taco {

ConstraintSetSetParser::ConstraintSetSetParser(Vocabulary &feature_set,
                                               Vocabulary &value_set)
    : ConstraintSetParserBase(feature_set, value_set) {
}

boost::shared_ptr<ConstraintSetSet> ConstraintSetSetParser::Parse(
    const StringPiece &s) {
  tokeniser_ = internal::ConstraintTokeniser(s);
  lookahead_ = *tokeniser_;
  boost::shared_ptr<ConstraintSetSet> css = NTConstraintSetSet();
  Match(internal::ConstraintToken_EOS);
  return css;
}

boost::shared_ptr<ConstraintSetSet>
    ConstraintSetSetParser::NTConstraintSetSet() {
  boost::shared_ptr<ConstraintSetSet> css(new ConstraintSetSet());
  if (lookahead_.type == internal::ConstraintToken_DIGITSEQ ||
      lookahead_.type == internal::ConstraintToken_LANGLE ||
      lookahead_.type == internal::ConstraintToken_STRING ||
      lookahead_.type == internal::ConstraintToken_LCURLY) {
    css->insert(NTConstraintSet());
    while (lookahead_.type == internal::ConstraintToken_COLON) {
      Match(internal::ConstraintToken_COLON);
      css->insert(NTConstraintSet());
    }
  }
  return css;
}

} // namespace taco
