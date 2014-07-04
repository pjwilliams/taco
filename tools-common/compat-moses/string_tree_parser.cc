#include "tools-common/compat-moses/string_tree_parser.h"

namespace taco {
namespace tool {
namespace moses {

void StringTreeNonTerminalLabeller::Label(const SyntaxNode &tree,
                                          StringTree &node) {
  SyntaxNode::AttributeSeqType::const_iterator p = tree.m_attributes.begin();
  SyntaxNode::AttributeSeqType::const_iterator p_end = tree.m_attributes.end();
  // TODO Binary search?
  for (; p != p_end; ++p) {
    if (p->first == "label") {
      node.label() = p->second;
      return;
    }
  }
}

void StringTreeTerminalLabeller::Label(const std::string &word,
                                       const SyntaxNode &,
                                       StringTree &node) {
  node.label() = word;
}

}  // namespace moses
}  // namespace tool
}  // namespace taco
