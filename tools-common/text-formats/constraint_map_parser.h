#ifndef TACO_TOOLS_COMMON_TEXT_FORMATS_CONSTRAINT_MAP_PARSER_H_
#define TACO_TOOLS_COMMON_TEXT_FORMATS_CONSTRAINT_MAP_PARSER_H_

#include "taco/base/string_piece.h"

#include <istream>
#include <string>
#include <vector>

namespace taco {
namespace tool {

// Parses the constraint map format produced by consolidate-constraints.
// Entries have a key and a set of IDs, which are indexes into an auxiliary
// constraint table.  The key is formed from the vocab IDs of the LHS and RHS
// symbols from of the target side of a SCFG rule, joined with the - character.
// For example,
//
//  100-10-4-58-2-9-382-4-187-7-2-9-159 ||| 914 101
//
// The auxiliary table can be parsed with ConstraintTableParser.
//
class ConstraintMapParser {
 public:
  struct Entry {
    Entry() {}
    std::string line;
    StringPiece key;
    std::vector<StringPiece> ids;
   private:
    // Copying is not allowed
    Entry(const Entry &);
    Entry &operator=(const Entry &);
  };

  ConstraintMapParser();
  ConstraintMapParser(std::istream &);

  ConstraintMapParser(const ConstraintMapParser &);
  ConstraintMapParser &operator=(const ConstraintMapParser &);

  const Entry &operator*() const { return value_; }
  const Entry *operator->() const { return &value_; }

  ConstraintMapParser &operator++();
  ConstraintMapParser operator++(int);

  friend bool operator==(const ConstraintMapParser &,
                         const ConstraintMapParser &);
  friend bool operator!=(const ConstraintMapParser &,
                         const ConstraintMapParser &);

 private:
  Entry value_;
  std::istream *input_;
  std::vector<StringPiece> tmp_vec_;

  void ParseLine(const StringPiece &);
};

}  // namespace tool
}  // namespace taco

#endif
