#ifndef TACO_TOOLS_COMBINE_CONSTRAINT_MAPS_INPUT_READER_H_
#define TACO_TOOLS_COMBINE_CONSTRAINT_MAPS_INPUT_READER_H_

#include "tools-common/text-formats/constraint_map_parser.h"

#include <string>

namespace taco {
namespace tool {

// Wrapper around ConstraintMapParser.  Checks order consistency to ensure that
// input was sorted according to the criteria that the matching algorithm
// assumes.
// TODO This is almost identical to MatchConstraintsToRules::InputReader.
// TODO Share code?
struct InputReader {
  InputReader(ConstraintMapParser, ConstraintMapParser, const std::string &);

  operator bool() const { return first != last; }

  const ConstraintMapParser::Entry &operator*() const { return *first; }
  const ConstraintMapParser::Entry *operator->() const { return &(*first); }

  bool ReadLine();

  ConstraintMapParser first;
  ConstraintMapParser last;
  std::string input_desc;
  std::size_t line_num;
  std::string key;
};

}  // namespace tool
}  // namespace taco

#endif
