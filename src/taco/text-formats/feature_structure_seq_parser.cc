#include "taco/text-formats/feature_structure_seq_parser.h"

#include <cassert>
#include <cctype>
#include <sstream>

#include "taco/base/exception.h"

namespace taco {

FeatureStructureSeqParser::FeatureStructureSeqParser() {}

FeatureStructureSeqParser::FeatureStructureSeqParser(const StringPiece &s,
                                                     Vocabulary &feature_set,
                                                     Vocabulary &value_set)
    : FeatureStructureParserBase(feature_set, value_set) {
  tokeniser_ = internal::FSTokeniser(s);
  lookahead_ = *tokeniser_;
  ++(*this);
}

FeatureStructureSeqParser &FeatureStructureSeqParser::operator++() {
  if (lookahead_.type == internal::FSToken_EOS) {
    value_.reset();
    // FIXME Match?
    return *this;
  }
  NTFS(spec_);
  value_.reset(new FeatureStructure(spec_));
  return *this;
}

FeatureStructureSeqParser FeatureStructureSeqParser::operator++(int) {
  FeatureStructureSeqParser tmp(*this);
  ++(*this);
  return tmp;
}

bool operator==(const FeatureStructureSeqParser &lhs,
                const FeatureStructureSeqParser &rhs) {
  if (!lhs.value_ || !rhs.value_) {
    return !lhs.value_ && !rhs.value_;
  }
  return lhs.tokeniser_ == rhs.tokeniser_;
}

bool operator!=(const FeatureStructureSeqParser &lhs,
                const FeatureStructureSeqParser &rhs) {
  return !(lhs == rhs);
}

} // namespace taco
