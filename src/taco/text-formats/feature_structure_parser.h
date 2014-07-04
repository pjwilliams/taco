#ifndef TACO_SRC_TACO_TEXT_FORMATS_FEATURE_STRUCTURE_PARSER_H_
#define TACO_SRC_TACO_TEXT_FORMATS_FEATURE_STRUCTURE_PARSER_H_

#include <boost/shared_ptr.hpp>

#include "taco/base/string_piece.h"
#include "taco/base/vocabulary.h"
#include "taco/feature_structure.h"
#include "taco/text-formats/feature_structure_tokeniser.h"

namespace taco {

////////////////////////////////////////////////////////////////////////////////
//
// Parses string representations of feature structures.  Accepts the following
// grammar:
//
//             FS -> AFS
//                 | CFS
//
//           AFS -> AtomicValue
//
//   AtomicValue -> Word
//
//           CFS -> '[' PairList ']'
//
//      PairList -> Pair PairList
//                | E
//
//          Pair -> Feature ':' FS
//
//       Feature -> Word
//
// Implemented as a predictive parser with one token of lookahead.
//
////////////////////////////////////////////////////////////////////////////////
namespace internal {
// The internal FeatureStructureParserBase class implements ConstraintParser
// other than providing the Parse() function.  It is also used as a base for
// FeatureStructureSeqParser.
class FeatureStructureParserBase {
 protected:
  FeatureStructureParserBase();
  FeatureStructureParserBase(Vocabulary &, Vocabulary &);

  StringPiece Match(FSTokenType);

  void NTFS(FeatureStructureSpec &);
  void NTCFS(FeatureStructureSpec &);
  void NTPairList(FeatureStructureSpec &);

  Vocabulary *feature_set_;
  Vocabulary *value_set_;
  FSTokeniser tokeniser_;
  FSToken lookahead_;
};
}  // namespace internal

class FeatureStructureParser : public internal::FeatureStructureParserBase {
 public:
  FeatureStructureParser(Vocabulary &, Vocabulary &);

  boost::shared_ptr<FeatureStructure> Parse(const StringPiece &);
  void Parse(const StringPiece &, FeatureStructureSpec &);
};

}  // namespace taco

#endif
