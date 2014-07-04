#ifndef TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_TABLE_PARSER_H_
#define TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_TABLE_PARSER_H_

#include <istream>
#include <string>

#include "taco/base/string_piece.h"

namespace taco {

class ConstraintTableParser {
 public:
  struct Entry {
    std::string line;
    StringPiece id;
    StringPiece constraint_set;
  };

  ConstraintTableParser();
  ConstraintTableParser(std::istream &);

  const Entry &operator*() const { return value_; }
  const Entry *operator->() const { return &value_; }

  ConstraintTableParser &operator++();
  ConstraintTableParser operator++(int);

  friend bool operator==(const ConstraintTableParser &,
                         const ConstraintTableParser &);
  friend bool operator!=(const ConstraintTableParser &,
                         const ConstraintTableParser &);

 private:
  Entry value_;
  std::istream *input_;

  void ParseLine(const StringPiece &);
};

}  // namespace taco

#endif
