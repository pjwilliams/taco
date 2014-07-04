#include "taco/text-formats/feature_structure_parser.h"

#include <cctype>

namespace taco {
namespace internal {

FeatureStructureParserBase::FeatureStructureParserBase()
    : feature_set_(0)
    , value_set_(0)
    , lookahead_(FSToken_EOS, "", -1) {
}

FeatureStructureParserBase::FeatureStructureParserBase(Vocabulary &feature_set,
                                                       Vocabulary &value_set)
    : feature_set_(&feature_set)
    , value_set_(&value_set)
    , lookahead_(FSToken_EOS, "", -1) {
}

StringPiece FeatureStructureParserBase::Match(FSTokenType expected_type) {
  if (lookahead_.type != expected_type) {
    std::ostringstream msg;
    msg << "Unexpected token: expected `" << expected_type
        << "', got `" << lookahead_.type << "'";
    throw Exception(msg.str());
  }
  StringPiece value = lookahead_.value;
  lookahead_ = *++tokeniser_;
  return value;
}

void FeatureStructureParserBase::NTFS(FeatureStructureSpec &spec) {
  spec.Clear();
  if (lookahead_.type == FSToken_WORD) {
    StringPiece word = Match(FSToken_WORD);
    AtomicValue atom = value_set_->Insert(word.as_string());
    FeaturePath path;
    spec.content_pairs.insert(std::make_pair(path, atom));
  } else {
    NTCFS(spec);
  }
}

void FeatureStructureParserBase::NTCFS(FeatureStructureSpec &spec) {
  spec.Clear();
  Match(internal::FSToken_LB);
  NTPairList(spec);
  if (spec.Empty()) {
    spec.content_pairs.insert(std::make_pair(FeaturePath(), kNullAtom));
  }
  Match(internal::FSToken_RB);
}

void FeatureStructureParserBase::NTPairList(FeatureStructureSpec &spec) {
  if (lookahead_.type != internal::FSToken_WORD) {
    return;
  }
  StringPiece s = Match(internal::FSToken_WORD);
  Feature feature = feature_set_->Insert(s.as_string());
  FeaturePath path(1, feature);
  Match(internal::FSToken_COLON);
  FeatureStructureSpec val_spec;
  NTFS(val_spec);
  if (val_spec.IsAtomic()) {
    AtomicValue atom = val_spec.content_pairs.begin()->second;
    spec.content_pairs.insert(std::make_pair(path, atom));
  } else {
    spec.Nest(path, val_spec);
  }
  NTPairList(spec);
}

}  // namespace internal

FeatureStructureParser::FeatureStructureParser(Vocabulary &feature_set,
                                               Vocabulary &value_set)
    : FeatureStructureParserBase(feature_set, value_set) {
}

boost::shared_ptr<FeatureStructure> FeatureStructureParser::Parse(
    const StringPiece &s) {
  tokeniser_ = internal::FSTokeniser(s);
  lookahead_ = *tokeniser_;
  FeatureStructureSpec spec;
  NTFS(spec);
  return boost::shared_ptr<FeatureStructure>(new FeatureStructure(spec));
}

void FeatureStructureParser::Parse(const StringPiece &s,
                                   FeatureStructureSpec &spec) {
  spec.Clear();
  tokeniser_ = internal::FSTokeniser(s);
  lookahead_ = *tokeniser_;
  NTFS(spec);
}

}  // namespace taco
