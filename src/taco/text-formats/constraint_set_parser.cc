#include "taco/text-formats/constraint_set_parser.h"

#include <cassert>
#include <cctype>
#include <sstream>

#include "taco/base/exception.h"

namespace taco {
namespace internal {

ConstraintSetParserBase::ConstraintSetParserBase() {}

ConstraintSetParserBase::ConstraintSetParserBase(Vocabulary &feature_set,
                                                 Vocabulary &value_set)
    : ConstraintParserBase(feature_set, value_set) {
}

boost::shared_ptr<ConstraintSet> ConstraintSetParserBase::NTConstraintSet() {
  boost::shared_ptr<ConstraintSet> cs(new ConstraintSet());
  // One or more constraints:
  ConstraintType type;
  boost::shared_ptr<Constraint> constraint = NTConstraint(type);
  cs->Insert(constraint, type);
  while (lookahead_.type == ConstraintToken_STRING ||
         lookahead_.type == ConstraintToken_LANGLE ||
         lookahead_.type == ConstraintToken_LCURLY) {
    constraint = NTConstraint(type);
    cs->Insert(constraint, type);
  }
  return cs;
}

} // namespace internal

ConstraintSetParser::ConstraintSetParser(Vocabulary &feature_set,
                                         Vocabulary &value_set)
    : ConstraintSetParserBase(feature_set, value_set) {
}

boost::shared_ptr<ConstraintSet> ConstraintSetParser::Parse(
    const StringPiece &s) {
  tokeniser_ = internal::ConstraintTokeniser(s);
  lookahead_ = *tokeniser_;
  boost::shared_ptr<ConstraintSet> cs = NTConstraintSet();
  Match(internal::ConstraintToken_EOS);
  return cs;
}

} // namespace taco
