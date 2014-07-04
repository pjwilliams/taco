#ifndef TACO_SRC_TACO_TEXT_FORMATS_FEATURE_TREE_PARSER_H_
#define TACO_SRC_TACO_TEXT_FORMATS_FEATURE_TREE_PARSER_H_

#include <cassert>
#include <cctype>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>

#include "taco/base/exception.h"
#include "taco/base/vocabulary.h"
#include "taco/feature_structure.h"

namespace taco {
namespace internal {

enum FTTokenType {
  FTToken_COLON,
  FTToken_EOS,
  FTToken_LB,
  FTToken_RB,
  FTToken_SEMICOLON,
  FTToken_WORD
};

struct FTToken {
 public:
  FTToken(FTTokenType t, const std::string &v, size_t p);

  FTTokenType type;
  std::string value;
  size_t pos;
};

class FTTokeniser {
 public:
  FTTokeniser();
  FTTokeniser(const std::string &);

  const FTToken &operator*() const { return m_value; }
  const FTToken *operator->() const { return &m_value; }

  FTTokeniser &operator++();
  FTTokeniser operator++(int);

  friend bool operator==(const FTTokeniser &, const FTTokeniser &);
  friend bool operator!=(const FTTokeniser &, const FTTokeniser &);

 private:
  FTToken m_value;
  const std::string *m_input;
  std::string::const_iterator m_iter;
  std::string::const_iterator m_end;
  size_t m_pos;
};

}  // namespace internal

////////////////////////////////////////////////////////////////////////////////
//
// Parses string representations of feature trees.  Accepts the following
// grammar:
//
//            FT -> '[' EntryList ']'
//
//     EntryList -> Entry EntryListTail
//                | E
//
// EntryListTail -> ';' EntryList
//                | E
//
//         Entry -> Feature EntryTail
//
//     EntryTail -> ':' FT
//                | E
//
//       Feature -> Word
//
// Implemented as a predictive parser with one token of lookahead.
//
////////////////////////////////////////////////////////////////////////////////
class FeatureTreeParser {
 public:
  FeatureTreeParser(Vocabulary &featureSet)
    : m_lookahead(internal::FTToken_EOS, "", -1)
    , m_featureSet(featureSet)
  {}

  ~FeatureTreeParser() {}

  boost::shared_ptr<FeatureTree> parse(const std::string &s) const;

 private:
  // Copying is not allowed.  Copy constructor and assignment operator are
  // not implemented.
  FeatureTreeParser(const FeatureTreeParser &);
  FeatureTreeParser &operator=(const FeatureTreeParser &);

  std::string match(internal::FTTokenType) const;

  boost::shared_ptr<FeatureTree> ntFT() const;
  void ntEntryList(FeatureTree &tree) const;
  void ntEntryListTail(FeatureTree &tree) const;
  void ntEntry(FeatureTree &tree) const;
  boost::shared_ptr<FeatureTree> ntEntryTail() const;

  mutable internal::FTToken m_lookahead;
  mutable internal::FTTokeniser m_tokeniser;
  Vocabulary &m_featureSet;
};

}  // namespace taco

#endif
