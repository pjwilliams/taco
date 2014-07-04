#ifndef TACO_TOOLS_COMMON_TEXT_FORMATS_CONSTRAINT_EXTRACT_PARSER_H_
#define TACO_TOOLS_COMMON_TEXT_FORMATS_CONSTRAINT_EXTRACT_PARSER_H_

#include "taco/base/string_piece.h"

#include <istream>
#include <string>
#include <vector>
#include <utility>

namespace taco {
namespace tool {

// Parses the constraint extract format produced by extract-constraints.
// Entries have a left hand side, a right hand side, and a sequence of disjoint
// constraint sets.  For example,
//
//  [NP] ||| der [NN] des [NN] ||| <1"AGR">=<2"AGR"> <1"AGR">="ART" : <3"AGR">=<4"AGR"> <3"AGR">="ART"
//
// Since both constraint sets and feature structures can potentially contain
// any character sequence (including spaces), there is no natural delimiter to
// use to separate elements.  ConstraintExtractParser therefore only assumes
// "|||" to be a delimiter and returns a single string for each sequence.
// Separation into individual elements should be performed by dedicated
// constraint set and feature structure parsers.
//
class ConstraintExtractParser {
 public:
  struct Entry {
    Entry() : is_identical_to_previous(false) {}
    std::string line;
    StringPiece lhs;
    std::vector<StringPiece> rhs;
    StringPiece constraint_sets;
    bool is_identical_to_previous;
    std::string tmp_line;
   private:
    // Copying is not allowed
    Entry(const Entry &);
    Entry &operator=(const Entry &);
  };

  ConstraintExtractParser();
  ConstraintExtractParser(std::istream &);

  const Entry &operator*() const { return value_; }
  const Entry *operator->() const { return &value_; }

  ConstraintExtractParser &operator++();

  friend bool operator==(const ConstraintExtractParser &,
                         const ConstraintExtractParser &);
  friend bool operator!=(const ConstraintExtractParser &,
                         const ConstraintExtractParser &);

 private:
  // Copying is not allowed
  ConstraintExtractParser(const ConstraintExtractParser &);
  ConstraintExtractParser &operator=(const ConstraintExtractParser &);

  Entry value_;
  std::istream *input_;

  void ParseLine(const StringPiece &);
};

}  // namespace tool
}  // namespace taco

#endif
