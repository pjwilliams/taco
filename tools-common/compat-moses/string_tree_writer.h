#ifndef TACO_TOOLS_COMMON_COMPAT_MOSES_STRING_TREE_WRITER_H_
#define TACO_TOOLS_COMMON_COMPAT_MOSES_STRING_TREE_WRITER_H_

#include "xml_tree_writer.h"

#include "taco/base/exception.h"
#include "tools-common/syntax-tree/string_tree.h"

#include <string>

namespace taco {
namespace tool {
namespace moses {

class StringTreeAttrMapGenerator {
 public:
  typedef std::map<std::string, std::string> AttrMap;
  void Generate(const StringTree &, AttrMap &);
};

class StringTreeWordGenerator {
 public:
  void Generate(const StringTree &, std::string &);
};

typedef XmlTreeWriter<StringTree,
                      StringTreeAttrMapGenerator,
                      StringTreeWordGenerator
                     > StringTreeWriter;

}  // namespace moses
}  // namespace tool
}  // namespace taco

#endif
