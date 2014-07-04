#ifndef TACO_TOOLS_COMMON_COMPAT_MOSES_XML_TREE_WRITER_DEFAULTS_H_
#define TACO_TOOLS_COMMON_COMPAT_MOSES_XML_TREE_WRITER_DEFAULTS_H_

#include "tools-common/compat-moses/xml_tree.h"
#include "tools-common/relation/relation_id.h"

#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>

#include <cassert>
#include <map>
#include <memory>
#include <ostream>
#include <vector>
#include <string>

namespace taco {
namespace tool {
namespace moses {

template<typename T>
class DefaultAttrMapGenerator {
 public:
  typedef std::map<std::string, std::string> AttrMap;
  typedef boost::function<void (const T &, AttrMap &)> AttrGeneratorType;
  void Generate(const T &, AttrMap &);
  void RegisterAttrGenerator(const AttrGeneratorType &);
 private:
  typedef std::vector<AttrGeneratorType> AttrWriterSeqType;
  AttrWriterSeqType attr_writer_seq_;
};

template<typename T>
class DefaultWordGenerator {
 public:
  typedef boost::function<void (const T &, std::string &)> WordGeneratorType;
  void Generate(const T &, std::string &);
  void RegisterWordGenerator(const WordGeneratorType &);
 private:
  WordGeneratorType word_generator_;
};

template<typename TreeType, int N, typename AttrType>
class TupleTreeAttrWriter {
 public:
  typedef typename DefaultAttrMapGenerator<TreeType>::AttrMap AttrMap;

  TupleTreeAttrWriter(const std::string &name) : name_(name) {}

  void operator()(const TreeType &node, AttrMap &attr_map) const {
    const AttrType &v = boost::tuples::get<N>(node.label());
    std::string s = boost::lexical_cast<std::string>(v);
    attr_map[name_] = s;
  }
 private:
  std::string name_;
};

// Specialisation of above for AttrType=std::string.
template<typename TreeType, int N>
class TupleTreeAttrWriter<TreeType, N, std::string> {
 public:
  typedef typename DefaultAttrMapGenerator<TreeType>::AttrMap AttrMap;

  TupleTreeAttrWriter(const std::string &name) : name_(name) {}

  void operator()(const TreeType &node, AttrMap &attr_map) const {
    const std::string &s = boost::tuples::get<N>(node.label());
    attr_map[name_] = s;
  }
 private:
  std::string name_;
};

// Specialisation of above for AttrType=RelationId
template<typename TreeType, int N>
class TupleTreeAttrWriter<TreeType, N, RelationId> {
 public:
  typedef typename DefaultAttrMapGenerator<TreeType>::AttrMap AttrMap;

  TupleTreeAttrWriter(const std::string &name) : name_(name) {}

  void operator()(const TreeType &node, AttrMap &attr_map) const {
    RelationId id = boost::tuples::get<N>(node.label());
    if (id >= 0) {
      std::string s = boost::lexical_cast<std::string>(id);
      attr_map[name_] = s;
    }
  }
 private:
  std::string name_;
};

// Specialisation of above for AttrType=std::string.
// TODO Can MapAttrType be inferred?
template<typename TreeType, int N, typename MapAttrType>
class TupleTreeMapWriter {
 public:
  typedef typename DefaultAttrMapGenerator<TreeType>::AttrMap AttrMap;

  void operator()(const TreeType &node, AttrMap &attr_map) const {
    const MapAttrType &v = boost::tuples::get<N>(node.label());
    for (typename MapAttrType::const_iterator p = v.begin();
         p != v.end(); ++p) {
      const std::string &name = p->first;
      const std::string &value = p->second;
      // TODO Should value be escaped here?
      attr_map[name] = value;
    }
  }
};

template<typename TreeType, int N>
class TupleTreeWordGenerator {
 public:
  void operator()(const TreeType &node, std::string &word) {
    word = boost::tuples::get<N>(node.label());
  }
};

template<typename T>
void DefaultAttrMapGenerator<T>::Generate(const T &node,
                                          AttrMap &attr_map) {
  for (typename AttrWriterSeqType::const_iterator p = attr_writer_seq_.begin();
       p != attr_writer_seq_.end(); ++p) {
    (*p)(node, attr_map);
  }
}

template<typename T>
void DefaultAttrMapGenerator<T>::RegisterAttrGenerator(
    const AttrGeneratorType &writer) {
  attr_writer_seq_.push_back(writer);
}

template<typename T>
void DefaultWordGenerator<T>::Generate(const T &node, std::string &w) {
  word_generator_(node, w);
}

template<typename T>
void DefaultWordGenerator<T>::RegisterWordGenerator(
    const WordGeneratorType &generator) {
  word_generator_ = generator;
}

}  // namespace moses
}  // namespace tool
}  // namespace taco

#endif
