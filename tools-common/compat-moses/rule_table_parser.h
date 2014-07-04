#ifndef TACO_TOOLS_COMMON_COMPAT_MOSES_RULE_TABLE_PARSER_H_
#define TACO_TOOLS_COMMON_COMPAT_MOSES_RULE_TABLE_PARSER_H_

#include "taco/base/string_piece.h"

#include <istream>
#include <string>
#include <vector>

namespace taco {
namespace tool {
namespace moses {

class RuleTableParser {
 public:
  // Flags to indicate which fields to extract and store in the Entry object.
  // By default every standard Moses field is extracted, if present (the source,
  // target, score, alignment, and count fields are obligatory; the rest are
  // not).  The constraint model fields CONSTRAINT_IDS and FEATURE_SELECTION_ID
  // are not extracted by default.  The line element is always included.
  // NOTE: A "field" refers to a "logical field" in the table.  The fields as
  // defined by the file format don't match up exactly: the first field in the
  // file contains both the source LHS and RHS, and similarly the second
  // contains both the target LHS and RHS.  But otherwise they do match.
  enum Field {
    SOURCE_LHS = 1,
    TARGET_LHS = 2,
    SOURCE_RHS = 4,
    TARGET_RHS = 8,
    SCORES = 16,
    ALIGNMENT = 32,
    COUNTS = 64,
    SPARSE_FEATURES = 128,
    KEY_VALUE_PAIRS = 256,
    CONSTRAINT_IDS = 512,
    FEATURE_SELECTION_ID = 1024,
    // All standard Moses values:
    ALL_MOSES = SOURCE_LHS | TARGET_LHS \
        | SOURCE_RHS | TARGET_RHS \
        | SCORES | ALIGNMENT | COUNTS | SPARSE_FEATURES | KEY_VALUE_PAIRS,
    // All standard Moses values + CM-specific values:
    ALL_CM = ALL_MOSES | CONSTRAINT_IDS | FEATURE_SELECTION_ID
  };

  // The parser processes a line at a time, storing the result in an Entry
  // value.  An Entry contains a full copy of the line plus StringPiece values
  // for the various fields.
  struct Entry {
   public:
    Entry() {}

    StringPiece source_lhs;
    StringPiece target_lhs;
    std::vector<StringPiece> source_rhs;
    std::vector<StringPiece> target_rhs;
    std::vector<StringPiece> scores;
    std::vector<std::pair<StringPiece,StringPiece> > alignment;
    std::vector<StringPiece> counts;
    StringPiece sparse_features;  // TODO Split?
    StringPiece key_value_pairs;  // TODO Split?
    std::vector<std::pair<StringPiece,StringPiece> > constraint_ids;
    StringPiece feature_selection_id;
    std::string line;
   private:
    // Copying is not allowed
    Entry(const Entry &);
    Entry &operator=(const Entry &);
  };

  RuleTableParser();
  RuleTableParser(std::istream &, int=ALL_MOSES);

  const Entry &operator*() const { return value_; }
  const Entry *operator->() const { return &value_; }

  RuleTableParser &operator++();
  //RuleTableParser operator++(int);

  friend bool operator==(const RuleTableParser &, const RuleTableParser &);
  friend bool operator!=(const RuleTableParser &, const RuleTableParser &);

 private:
  // Copying is not allowed
  RuleTableParser(const RuleTableParser &);
  RuleTableParser &operator=(const RuleTableParser &);

  void ParseLine(StringPiece);

  static std::size_t ParseField(StringPiece, std::size_t, int, bool, bool,
                                StringPiece &);

  static std::size_t ParseField(StringPiece, std::size_t, int, bool, bool,
                                std::vector<StringPiece> &);

  std::size_t ParseField(StringPiece, std::size_t, int, bool, bool,
      void (*)(const StringPiece &, std::pair<StringPiece,StringPiece> &),
      std::vector<std::pair<StringPiece,StringPiece> > &);

  static void TrimPairedSymbolFromLeft(StringPiece &);

  static void TrimPairedSymbolFromRight(StringPiece &);

  static void SplitAlignmentPair(const StringPiece &,
                                 std::pair<StringPiece,StringPiece> &);

  static void SplitConstraintId(const StringPiece &,
                                std::pair<StringPiece,StringPiece> &);

  static std::string Ordinal(int);

  int fields_;
  Entry value_;
  std::istream *input_;
  std::vector<StringPiece> tmp_vec_;
};

}  // namespace moses
}  // namespace tool
}  // namespace taco

#endif
