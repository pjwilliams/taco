#ifndef TACO_TOOLS_COMMON_TEXT_FORMATS_RULE_TABLE_INDEX_PARSER_H_
#define TACO_TOOLS_COMMON_TEXT_FORMATS_RULE_TABLE_INDEX_PARSER_H_

#include "taco/base/string_piece.h"

#include <istream>
#include <string>
#include <vector>

namespace taco {
namespace tool {

// Parse the rule table index format produced by index-rule-table.
// Entries have left hand and right hand symbols (from the target side of a
// SCFG) and a rule table line number.  For example,
//
// [AA-HD] ||| [PTKA] besten ||| 116039061
//
class RuleTableIndexParser {
 public:
  struct Entry {
    Entry() {}
    std::string line;
    StringPiece key;
    int line_num;
   private:
    // Copying is not allowed
    Entry(const Entry &);
    Entry &operator=(const Entry &);
  };

  RuleTableIndexParser();
  RuleTableIndexParser(std::istream &);

  RuleTableIndexParser(const RuleTableIndexParser &);
  RuleTableIndexParser &operator=(const RuleTableIndexParser &);

  const Entry &operator*() const { return value_; }
  const Entry *operator->() const { return &value_; }

  RuleTableIndexParser &operator++();

  friend bool operator==(const RuleTableIndexParser &,
                         const RuleTableIndexParser &);
  friend bool operator!=(const RuleTableIndexParser &,
                         const RuleTableIndexParser &);

 private:
  Entry value_;
  std::istream *input_;

  void ParseLine(StringPiece);
};

}  // namespace tool
}  // namespace taco

#endif
