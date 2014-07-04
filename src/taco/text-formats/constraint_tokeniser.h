#ifndef TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_TOKENISER_H_
#define TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_TOKENISER_H_

#include <string>

#include "taco/base/string_piece.h"
#include "taco/base/vocabulary.h"
#include "taco/constraint.h"

namespace taco {
namespace internal {

enum ConstraintTokenType {
  ConstraintToken_COLON,
  ConstraintToken_COMMA,
  ConstraintToken_DIGITSEQ,
  ConstraintToken_EOS,
  ConstraintToken_EQUALS,
  ConstraintToken_LANGLE,
  ConstraintToken_LCURLY,
  ConstraintToken_PROBABILITY,
  ConstraintToken_RANGLE,
  ConstraintToken_RCURLY,
  ConstraintToken_STRING,
  ConstraintToken_VARNAME
};

const std::string &GetConstraintTokenTypeName(ConstraintTokenType);

struct ConstraintToken {
 public:
  ConstraintToken(ConstraintTokenType, const std::string &, size_t);
  ConstraintTokenType type;
  // Note: value needs to be a full string.  It can't be a StringPiece
  // because the input might contain escaped characters that need to
  // be unescaped to produce the token value.
  std::string value;
  size_t pos;
};

class ConstraintTokeniser {
 public:
  ConstraintTokeniser();
  ConstraintTokeniser(const StringPiece &);

  const ConstraintToken &operator*() const { return value_; }
  const ConstraintToken *operator->() const { return &value_; }

  ConstraintTokeniser &operator++();
  ConstraintTokeniser operator++(int);

  friend bool operator==(const ConstraintTokeniser &,
                         const ConstraintTokeniser &);
  friend bool operator!=(const ConstraintTokeniser &,
                         const ConstraintTokeniser &);

 private:
  StringPiece str_;
  ConstraintToken value_;
  StringPiece::const_iterator iter_;
  StringPiece::const_iterator end_;
  size_t pos_;
};

}  // namespace internal
}  // namespace taco

#endif
