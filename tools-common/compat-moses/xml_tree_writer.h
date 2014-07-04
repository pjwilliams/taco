#ifndef TACO_TOOLS_COMMON_COMPAT_MOSES_XML_TREE_WRITER_H_
#define TACO_TOOLS_COMMON_COMPAT_MOSES_XML_TREE_WRITER_H_

//#include "tools-common/compat-moses/xml_tree.h"
#include "tools-common/compat-moses/xml_tree_writer_defaults.h"

#include <cassert>
#include <map>
#include <ostream>
#include <vector>
#include <string>

namespace taco {
namespace tool {
namespace moses {

template<typename T,
         typename AMG=DefaultAttrMapGenerator<T>,
         typename WG=DefaultWordGenerator<T> >
class XmlTreeWriter : public AMG, public WG {
 public:
  typedef AMG AttrMapGenerator;
  typedef WG WordGenerator;
  typedef std::map<std::string, std::string> AttrMap;

  void Write(const T &, std::ostream &);

 private:
  std::string Escape(const std::string &) const;
};

template<typename T, typename AMG, typename WG>
void XmlTreeWriter<T,AMG,WG>::Write(const T &node, std::ostream &out) {
  assert(!node.IsLeaf());

  // Opening tag

  out << "<tree";

  AttrMap attr_map;
  AttrMapGenerator::Generate(node, attr_map);

  // FIXME For compatibility with the old XmlTreeWriter class, "label" is
  // always written first.  This can be changed when XmlTreeWriter is removed.
  AttrMap::const_iterator q = attr_map.find("label");
  assert(q != attr_map.end());
  out << " " << q->first << "=\"" << q->second << "\"";
  for (AttrMap::const_iterator p = attr_map.begin(); p != attr_map.end(); ++p) {
    if (p->first == "label") {
      continue;
    }
    // TODO Escape name?  Escape value?
    out << " " << p->first << "=\"" << p->second << "\"";
  }

  out << ">";

  // Children

  const std::vector<T *> &children = node.children();
  for (typename std::vector<T *>::const_iterator p = children.begin();
       p != children.end(); ++p) {
    T &child = **p;
    if (child.IsLeaf()) {
      // TODO Handle case that label isn't std::string
      std::string s;
      WordGenerator::Generate(child, s);
      out << " " << s;  // Escape(s);
    } else {
      out << " ";
      Write(**p, out);
    }
  }

  // Closing tag
  out << " </tree>";

  if (node.parent() == 0) {
    out << std::endl;
  }
}

// Escapes XML special characters.
template<typename T, typename AMG, typename WG>
std::string XmlTreeWriter<T,AMG,WG>::Escape(const std::string &s) const {
  std::string t;
  size_t len = s.size();
  t.reserve(len);
// TODO Add new escape sequences
  for (size_t i = 0; i < len; ++i) {
    if (s[i] == '<') {
      t += "&lt;";
    } else if (s[i] == '>') {
      t += "&gt;";
    } else if (s[i] == '&') {
      t += "&amp;";
    } else if (s[i] == '\'') {
      t += "&apos;";
    } else if (s[i] == '"') {
      t += "&quot;";
    } else {
      t += s[i];
    }
  }
  return t;
}

}  // namespace moses
}  // namespace tool
}  // namespace taco

#endif
