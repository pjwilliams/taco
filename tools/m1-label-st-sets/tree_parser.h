#ifndef TACO_TOOLS_M1_LABEL_ST_SETS_TREE_PARSER_H_
#define TACO_TOOLS_M1_LABEL_ST_SETS_TREE_PARSER_H_

#include "typedef.h"

#include "tools-common/compat-moses/xml_tree_parser.h"
#include "tools-common/relation/relation_id.h"

namespace taco {
namespace tool {
namespace m1 {

class TreeParser {
 public:
  TreeParser(const std::string &relation_label) {
    parser_.RegisterMatchCallback("label", cat_match_cb_);
    parser_.RegisterMatchCallback(relation_label, id_match_cb_);
    parser_.RegisterMatchCallback(default_cb_);
    parser_.RegisterTerminalCallback(cat_terminal_cb_);
    parser_.RegisterTerminalCallback(id_terminal_cb_);
  }

  std::auto_ptr<Tree> Parse(const std::string &s) { return parser_.Parse(s); }

 private:
  moses::XmlTreeParser<Tree> parser_;

  // Non-terminal callbacks for parser_.
  moses::TupleTreeAttrAssigner<Tree, kIdxCat, std::string> cat_match_cb_;
  moses::TupleTreeAttrAssigner<Tree, kIdxId, RelationId> id_match_cb_;
  moses::TupleTreeAttrInserter<Tree, kIdxUnused> default_cb_;

  // Terminal callbacks for parser_.
  moses::TupleTreeWordAssigner<Tree, kIdxCat> cat_terminal_cb_;
  moses::TupleTreeInheritor<Tree, kIdxId, RelationId> id_terminal_cb_;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
