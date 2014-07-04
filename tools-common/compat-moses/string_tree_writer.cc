#include "tools-common/compat-moses/string_tree_writer.h"

namespace taco {
namespace tool {
namespace moses {

void StringTreeAttrMapGenerator::Generate(const StringTree &t,
                                          AttrMap &attr_map) {
  attr_map["label"] = t.label();
}

void StringTreeWordGenerator::Generate(const StringTree &t, std::string &word) {
  word = t.label();
}

}  // namespace moses
}  // namespace tool
}  // namespace taco
