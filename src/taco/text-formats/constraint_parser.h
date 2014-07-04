#ifndef TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_PARSER_H_
#define TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_PARSER_H_

#include <boost/shared_ptr.hpp>

#include "taco/base/string_piece.h"
#include "taco/base/vocabulary.h"
#include "taco/constraint.h"
#include "taco/text-formats/constraint_tokeniser.h"

namespace taco {

////////////////////////////////////////////////////////////////////////////
//
// Class for parsing a string representation of a single constraint, which is
// expected to have the following form:
//
//             Constraint -> AbsConstraint
//                         | RelConstraint
//                         | VarConstraint
//
//          AbsConstraint -> PathTerm EQUALS ValueTerm
//                         | ValueTerm EQUALS PathTerm
//
//          RelConstraint -> PathTerm EQUALS PathTerm
//
//          VarConstraint -> PathTerm EQUALS VarTerm
//                         | VarTerm EQUALS PathTerm
//
//               PathTerm -> LANGLE DIGITSEQ Path RANGLE
//
//              ValueTerm -> STRING
//
//                VarTerm -> ProbFunc
//
//                   Path -> STRING Path
//                         | E
//
//               ProbFunc -> LCURLY ValProbList RCURLY
//
//            ValProbList -> STRING COLON PROBABILITY ValProbListTail
//                         | E
//
//        ValProbListTail -> COMMA STRING COLON PROBABILITY ValProbListTail
//                         | E
//
// Whitespace is optional and is ignored.
//
// The implementation is a predictive parser with one token of lookahead.
//
////////////////////////////////////////////////////////////////////////////
namespace internal {
// The internal ConstraintParserBase class implements ConstraintParser except
// for providing the Parse() function.  It is also used as a base for
// ConstraintSetParser and ConstraintSetSetParser.
class ConstraintParserBase {
 protected:
  ConstraintParserBase();
  ConstraintParserBase(Vocabulary &, Vocabulary &);

  void Match(ConstraintTokenType);
  void Match(ConstraintTokenType, std::string &);

  boost::shared_ptr<Constraint> NTConstraint(ConstraintType &);
  PathTerm NTPathTerm();
  ValueTerm NTValueTerm();
  VarTerm NTVarTerm();
  void NTPath(FeaturePath &);
  void NTValProbList(VarTerm::ProbabilityMap &);
  void NTValProbListTail(VarTerm::ProbabilityMap &);

  Vocabulary *feature_set_;
  Vocabulary *value_set_;
  ConstraintTokeniser tokeniser_;
  ConstraintToken lookahead_;
  std::string tmp_value_;
};
}  // namespace internal

class ConstraintParser : public internal::ConstraintParserBase {
 public:
  ConstraintParser(Vocabulary &, Vocabulary &);
  boost::shared_ptr<Constraint> Parse(const StringPiece &s,
                                      ConstraintType &);
};

}  // namespace taco

#endif
