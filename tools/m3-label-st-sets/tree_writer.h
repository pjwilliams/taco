#ifndef TACO_TOOLS_M3_LABEL_ST_SETS_TREE_WRITER_H_
#define TACO_TOOLS_M3_LABEL_ST_SETS_TREE_WRITER_H_

#include <ostream>
#include <string>

#include "tools-common/compat-moses/xml_tree_writer.h"

#include "typedef.h"

namespace taco {
namespace tool {
namespace m3 {

class TreeWriter {
 public:
  TreeWriter(const std::string &relation_label)
      : cat_writer_cb_("label")
      , id_writer_cb_(relation_label) {
    writer_.RegisterAttrGenerator(cat_writer_cb_);
    writer_.RegisterAttrGenerator(id_writer_cb_);
    writer_.RegisterAttrGenerator(unused_writer_cb_);
    writer_.RegisterWordGenerator(word_generator_);
  }

  void Write(const Tree &tree, std::ostream &out) { writer_.Write(tree, out); }

 private:
  moses::XmlTreeWriter<Tree> writer_;

  // Non-terminal callbacks for writer_.
  moses::TupleTreeAttrWriter<Tree, kIdxCat, std::string> cat_writer_cb_;
  moses::TupleTreeAttrWriter<Tree, kIdxId, RelationId> id_writer_cb_;
  moses::TupleTreeMapWriter<Tree, kIdxUnused, AttrMap> unused_writer_cb_;

  // Terminal callbacks for writer_.
  moses::TupleTreeWordGenerator<Tree, kIdxCat> word_generator_;
};

}  // namespace m3
}  // namespace tool
}  // namespace taco

#endif
