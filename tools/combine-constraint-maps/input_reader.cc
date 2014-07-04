#include "input_reader.h"

#include "tools-common/text-formats/constraint_map_parser.h"

#include "taco/base/exception.h"
#include "taco/base/string_piece.h"

#include <sstream>

namespace taco {
namespace tool {

InputReader::InputReader(ConstraintMapParser f,
                         ConstraintMapParser l,
                         const std::string &d)
    : first(f)
    , last(l)
    , input_desc(d)
    , line_num(1) {
  if (first != last) {
    key = first->key.as_string();
  }
}

bool InputReader::ReadLine() {
  if (++first == last) {
    return false;
  }
  ++line_num;
  std::string new_key(first->key.as_string());
  if (new_key.compare(key) <= 0) {
    std::ostringstream msg;
    msg << input_desc << " is not correctly ordered between lines "
        << line_num-1 << " and " << line_num << std::endl;
    throw Exception(msg.str());
  }
  key = new_key;
  return true;
}

}  // namespace tool
}  // namespace taco
