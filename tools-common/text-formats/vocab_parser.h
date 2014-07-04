#ifndef TACO_TOOLS_COMMON_TEXT_FORMATS_VOCAB_PARSER_H_
#define TACO_TOOLS_COMMON_TEXT_FORMATS_VOCAB_PARSER_H_

#include "taco/base/string_piece.h"

#include <istream>
#include <string>
#include <vector>

namespace taco {
namespace tool {

// Parses the vocab format produced by extract-vocab.
// Entries have a symbol (terminal or non-terminal), a total count, a set of
// POS labels, and a set of POS counts.  The POS label and count sets are
// empty for non-terminals.  For terminals, they are in descending order.
// For example,
//
// TODO Use a real terminal example
//
//   [NN] ||| 10544153 ||| |||
//   an ||| 160304 ||| APPR ADV APZR PTKVZ ||| 160301 1 1 1
//
class VocabParser {
 public:
  struct Entry {
    Entry() {}
    std::string line;
    StringPiece symbol;
    StringPiece count;
    std::vector<StringPiece> pos_set;
    std::vector<StringPiece> pos_counts;
   private:
    // Copying is not allowed
    Entry(const Entry &);
    Entry &operator=(const Entry &);
  };

  VocabParser();
  VocabParser(std::istream &);

  const Entry &operator*() const { return value_; }
  const Entry *operator->() const { return &value_; }

  VocabParser &operator++();

  friend bool operator==(const VocabParser &, const VocabParser &);
  friend bool operator!=(const VocabParser &, const VocabParser &);

 private:
  // Copying is not allowed
  VocabParser(const VocabParser &);
  VocabParser &operator=(const VocabParser &);

  void ParseLine(const StringPiece &);

  Entry value_;
  std::istream *input_;
};

}  // namespace tool
}  // namespace taco

#endif
