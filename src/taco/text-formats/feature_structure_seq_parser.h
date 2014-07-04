#ifndef TACO_SRC_TACO_TEXT_FORMATS_FEATURE_STRUCTURE_SEQ_PARSER_H_
#define TACO_SRC_TACO_TEXT_FORMATS_FEATURE_STRUCTURE_SEQ_PARSER_H_

#include <string>

#include "taco/base/vocabulary.h"
#include "taco/feature_structure.h"
#include "taco/text-formats/feature_structure_parser.h"

namespace taco {

////////////////////////////////////////////////////////////////////////////////
// Class for parsing a string representation of a feature structure seq, which
// is expected to have the following form:
//
//                     FSSeq -> FS Tail
//                            | E
//
//                      Tail -> COLON FS Tail
//                            | E
//
// Where a FS has the same form as for FeatureStructureParser.  Whitespace is
// optional and ignored.
//
////////////////////////////////////////////////////////////////////////////////
class FeatureStructureSeqParser : public internal::FeatureStructureParserBase {
 public:
  FeatureStructureSeqParser();
  FeatureStructureSeqParser(const StringPiece &, Vocabulary &, Vocabulary &);

  const boost::shared_ptr<FeatureStructure> &operator*() const {
    return value_;
  }
  const boost::shared_ptr<FeatureStructure> *operator->() const {
    return &value_;
  }

  FeatureStructureSeqParser &operator++();
  FeatureStructureSeqParser operator++(int);

  friend bool operator==(const FeatureStructureSeqParser &,
                         const FeatureStructureSeqParser &);
  friend bool operator!=(const FeatureStructureSeqParser &,
                         const FeatureStructureSeqParser &);

 private:
  boost::shared_ptr<FeatureStructure> value_;
  FeatureStructureSpec spec_;
};

}  // namespace taco

#endif
