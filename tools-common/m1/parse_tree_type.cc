#include "tools-common/m1/parse_tree_type.h"

#include <string>

namespace taco {
namespace tool {
namespace m1 {

bool StrToParseTreeType(const std::string &s, ParseTreeType &t) {
  if (s == "bitpar") {
    t = kBitPar;
    return true;
  }
  if (s == "parzu") {
    t = kParZu;
    return true;
  }
  return false;
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
