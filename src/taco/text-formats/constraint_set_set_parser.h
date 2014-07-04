#ifndef TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_SET_SET_PARSER_H_
#define TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_SET_SET_PARSER_H_

#include <string>

#include "taco/base/vocabulary.h"
#include "taco/constraint.h"
#include "taco/constraint_set.h"
#include "taco/constraint_set_set.h"
#include "taco/text-formats/constraint_set_parser.h"

namespace taco {

////////////////////////////////////////////////////////////////////////////////
// Class for parsing a string representation of a constraint set set, which is
// expected to have the following form:
//
//          ConstraintSetSet -> ConstraintSet CSSTail
//                            | E
//
//                   CSSTail -> COLON ConstraintSet CSSTail
//                            | E
//
// Where a ConstraintSet has the same form as for ConstraintSetParser.
// Whitespace is optional and ignored.
//
////////////////////////////////////////////////////////////////////////////////
class ConstraintSetSetParser : public internal::ConstraintSetParserBase {
 public:
  ConstraintSetSetParser(Vocabulary &, Vocabulary &);
  boost::shared_ptr<ConstraintSetSet> Parse(const StringPiece &s);
 private:
  boost::shared_ptr<ConstraintSetSet> NTConstraintSetSet();
};

}  // namespace taco

#endif
