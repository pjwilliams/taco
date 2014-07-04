#ifndef TACO_TOOLS_COMMON_COMPAT_MOSES_STRING_TREE_PARSER_H_
#define TACO_TOOLS_COMMON_COMPAT_MOSES_STRING_TREE_PARSER_H_

#include "xml_tree_parser.h"

#include "taco/base/exception.h"
#include "tools-common/syntax-tree/string_tree.h"

#include <string>

namespace taco {
namespace tool {
namespace moses {

class StringTreeNonTerminalLabeller {
 public:
  void Label(const SyntaxNode &, StringTree &);
};

class StringTreeTerminalLabeller {
 public:
  void Label(const std::string &, const SyntaxNode &, StringTree &);
};

typedef XmlTreeParser<StringTree,
                      StringTreeNonTerminalLabeller,
                      StringTreeTerminalLabeller
                     > StringTreeParser;

}  // namespace moses
}  // namespace tool
}  // namespace taco

#endif
