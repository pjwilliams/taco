#ifndef TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_SET_PARSER_H_
#define TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_SET_PARSER_H_

#include <string>

#include "taco/base/vocabulary.h"
#include "taco/constraint.h"
#include "taco/constraint_set.h"
#include "taco/text-formats/constraint_parser.h"

namespace taco {

////////////////////////////////////////////////////////////////////////////////
// Class for parsing a string representation of a constraint set, which is
// expected to have the following form:
//
//          ConstraintSet -> Constraint CSTail
//
//                 CSTail -> Constraint CSTail
//                         | E
//
// Where a constraint.has the same form as for ConstraintParser.  Whitespace is
// optional and ignored.
//
////////////////////////////////////////////////////////////////////////////////
namespace internal {
// The internal ConstraintSetParserBase class implements ConstraintSetParser
// except for providing the Parse() function.  It is also used as a base for
// ConstraintSetSetParser and ConstraintSetSeqParser.
class ConstraintSetParserBase : public internal::ConstraintParserBase {
 protected:
  ConstraintSetParserBase();
  ConstraintSetParserBase(Vocabulary &, Vocabulary &);
  boost::shared_ptr<ConstraintSet> NTConstraintSet();
};
}  // namespace internal

class ConstraintSetParser : public internal::ConstraintSetParserBase {
 public:
  ConstraintSetParser(Vocabulary &, Vocabulary &);
  boost::shared_ptr<ConstraintSet> Parse(const StringPiece &s);
};

}  // namespace taco

#endif
