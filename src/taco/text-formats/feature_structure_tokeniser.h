#ifndef TACO_SRC_TACO_TEXT_FORMATS_FEATURE_STRUCTURE_TOKENISER_H_
#define TACO_SRC_TACO_TEXT_FORMATS_FEATURE_STRUCTURE_TOKENISER_H_

#include <string>

#include "taco/base/string_piece.h"

namespace taco {
namespace internal {

enum FSTokenType {
  FSToken_COLON,
  FSToken_EOS,
  FSToken_LB,
  FSToken_RB,
  FSToken_WORD
};

struct FSToken {
 public:
  FSToken(FSTokenType, StringPiece, size_t);
  FSTokenType type;
  StringPiece value;
  size_t pos;
};

class FSTokeniser {
 public:
  FSTokeniser();
  FSTokeniser(const StringPiece &);

  const FSToken &operator*() const { return value_; }
  const FSToken *operator->() const { return &value_; }

  FSTokeniser &operator++();
  FSTokeniser operator++(int);

  friend bool operator==(const FSTokeniser &, const FSTokeniser &);
  friend bool operator!=(const FSTokeniser &, const FSTokeniser &);

 private:
  StringPiece str_;
  FSToken value_;
  StringPiece::const_iterator iter_;
  StringPiece::const_iterator end_;
  size_t pos_;
};

}  // namespace internal
}  // namespace taco

#endif
