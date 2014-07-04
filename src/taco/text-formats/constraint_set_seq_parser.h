#ifndef TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_SET_SEQ_PARSER_H_
#define TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_SET_SEQ_PARSER_H_

#include <string>

#include "taco/base/vocabulary.h"
#include "taco/constraint.h"
#include "taco/constraint_set.h"
#include "taco/constraint_set_set.h"
#include "taco/text-formats/constraint_set_parser.h"

namespace taco {

////////////////////////////////////////////////////////////////////////////////
// Class for parsing a string representation of a constraint set seq, which is
// expected to have the following form:
//
//          ConstraintSetSeq -> ConstraintSet CSSTail
//                            | E
//
//                   CSSTail -> COLON ConstraintSet CSSTail
//                            | E
//
// Where a ConstraintSet has the same form as for ConstraintSetParser.
// Whitespace is optional and ignored.
//
////////////////////////////////////////////////////////////////////////////////
class ConstraintSetSeqParser : public internal::ConstraintSetParserBase {
 public:
  ConstraintSetSeqParser();
  ConstraintSetSeqParser(const StringPiece &, Vocabulary &, Vocabulary &);

  const boost::shared_ptr<ConstraintSet> &operator*() const { return value_; }
  const boost::shared_ptr<ConstraintSet> *operator->() const { return &value_; }

  ConstraintSetSeqParser &operator++();
  ConstraintSetSeqParser operator++(int);

  friend bool operator==(const ConstraintSetSeqParser &,
                         const ConstraintSetSeqParser &);
  friend bool operator!=(const ConstraintSetSeqParser &,
                         const ConstraintSetSeqParser &);

 private:
  boost::shared_ptr<ConstraintSet> value_;
};

}  // namespace taco

#endif
