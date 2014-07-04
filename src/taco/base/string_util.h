#ifndef TACO_SRC_TACO_BASE_STRING_UTIL_H_
#define TACO_SRC_TACO_BASE_STRING_UTIL_H_

#include <string>
#include <vector>

#include "taco/base/string_piece.h"

namespace taco {

// Variant of Moses' Tokenize() that uses StringPiece.
void Tokenize(std::vector<StringPiece> &output, StringPiece str,
              const std::string &delimiters = " \t");

// Variant of Moses' TokenizeMultiCharSeparator() that uses StringPiece.
void TokenizeMultiCharSeparator(std::vector<StringPiece> &output,
                                StringPiece str,
                                const std::string &separator = "|||");

void Trim(StringPiece &str, const std::string &chars =" \t");

}  // namespace taco

#endif
