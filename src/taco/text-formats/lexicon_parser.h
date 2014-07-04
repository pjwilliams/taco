#ifndef TACO_SRC_TACO_TEXT_FORMATS_LEXICON_PARSER_H_
#define TACO_SRC_TACO_TEXT_FORMATS_LEXICON_PARSER_H_

#include <istream>
#include <string>

#include "taco/base/string_piece.h"

namespace taco {

// Parse the lexicon file format.  Each line either contains an entry or is
// 'blank.'  An entry has a word and an FS.  For example:
//
//   mit ||| [AGR:[CASE:dat];POS:APPR]
//
// Blank lines are lines that contain only whitespace or comments (introduced
// with '#').  Comments are only allowed on blank lines.
//
class LexiconParser {
 public:
  struct Entry {
    std::string line;
    std::string word;
    std::string fs;
  };

  LexiconParser();
  LexiconParser(std::istream &);

  const Entry &operator*() const { return value_; }
  const Entry *operator->() const { return &value_; }

  LexiconParser &operator++();
  LexiconParser operator++(int);

  friend bool operator==(const LexiconParser &, const LexiconParser &);
  friend bool operator!=(const LexiconParser &, const LexiconParser &);

 private:
  bool IsBlankLine(const std::string &);
  void ParseLine(const StringPiece &);

  Entry value_;
  std::istream *input_;
};

}  // namespace taco

#endif
