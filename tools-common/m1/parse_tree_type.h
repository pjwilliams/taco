#ifndef TACO_TOOLS_COMMON_M1_PARSE_TREE_TYPE_H_
#define TACO_TOOLS_COMMON_M1_PARSE_TREE_TYPE_H_

#include <string>

namespace taco {
namespace tool {
namespace m1 {

enum ParseTreeType {
  kBitPar,
  kParZu
};

bool StrToParseTreeType(const std::string &, ParseTreeType &);

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
