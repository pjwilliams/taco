#ifndef TACO_TOOLS_COMMON_COMPAT_MOSES_XML_TREE_PARSER_DEFAULTS_H_
#define TACO_TOOLS_COMMON_COMPAT_MOSES_XML_TREE_PARSER_DEFAULTS_H_

#include "taco/base/exception.h"

#include "tools-common/compat-moses/syntax_tree.h"

#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace taco {
namespace tool {
namespace moses {

// DefaultNonTerminalLabeller performs actions triggered by the presence or
// absence of XML attributes with specified names.  The behaviour is
// configured by registering name-functor pairs that associate callback
// functions with the the attributes of interest.  An additional 'fallback'
// functor can be registered for attributes that don't have their own
// dedicated callback.
template<typename T>
class DefaultNonTerminalLabeller {
 public:
  // Define the type of the callback function used for present attributes.
  // The first two arguments are the attribute name and value (unescaped),
  // the third is the tree that has been created for this non-terminal node.
  // TODO Should the callback receive the original Moses::SyntaxNode as well?
  typedef boost::function<void (const std::string &,
                                const std::string &,
                                T &) > MatchCallbackType;

  // Define the type of the callback function used for missing attributes.
  // The first argument is the attribute name, the second is the tree that
  // has been created for this non-terminal node.
  // TODO Should the callback receive the original Moses::SyntaxNode as well?
  typedef boost::function<void (const std::string &,
                                T &) > MissingCallbackType;

  DefaultNonTerminalLabeller();

  void Label(const SyntaxNode &, T &);

  // Register a functor to be called when an attribute with the specified name
  // is found.
  void RegisterMatchCallback(const std::string &, const MatchCallbackType &);

  // Register a functor to be called if an attribute is found that does not
  // have a dedicated callback.
  void RegisterMatchCallback(const MatchCallbackType &);

  // Register a functor to be called if no attribute is found with the specified
  // name.
  void RegisterMissingCallback(const std::string &,
                               const MissingCallbackType &);

 private:
  typedef std::map<std::string, MatchCallbackType> MatchMapType;
  typedef std::map<std::string, MissingCallbackType> MissingMapType;

  // Define the default, no-op callback that is used if an attribute has no
  // dedicated callback.
  static void NullDefaultCallback(const std::string &, const std::string &,
                                  T &) {}

  MatchMapType match_cb_map_;
  MatchCallbackType default_cb_;
  MissingMapType missing_cb_map_;
};

// DefaultTerminalLabeller labels a T node by invoking a sequence of one or
// more functors that perform the work.  These have access to the word from
// the XML input and to its parent SyntaxNode.
template<typename T>
class DefaultTerminalLabeller {
 public:
  typedef boost::function<void (const std::string &, const SyntaxNode &,
                                T &)> CallbackType;

  void Label(const std::string &, const SyntaxNode &, T &);

  void RegisterTerminalCallback(const CallbackType &);

 private:
  typedef std::vector<CallbackType> CallbackSeqType;
  CallbackSeqType callback_seq_;
};

// Defines a match callback for use with DefaultNonTerminalLabeller.  Assumes
// the type of TreeType is SyntaxTree<boost::tuple<...> > and that the
// attribute corresponds to the tuple element N, which is of type ElementType.
// TODO specialize for ElementType=std::string?
template<typename TreeType, int N, typename ElementType>
class TupleTreeAttrAssigner {
 public:
  void operator()(const std::string &name, const std::string &value,
                  TreeType &node) const {
    // WARNING: the value is assumed to have been unescaped.
    ElementType v = boost::lexical_cast<ElementType>(value);
    boost::tuples::get<N>(node.label()) = v;
  }
};

// Defines a match callback for use with DefaultNonTerminalLabeller.  Assumes
// the type of TreeType is SyntaxTree<boost::tuple<...> > and that tuple
// element N is a std::map<std::string, std::string> or provides a compatible
// interface.  The attribute value is inserted into the map using the name
// as the key.
template<typename TreeType, int N>
class TupleTreeAttrInserter {
 public:
  void operator()(const std::string &name, const std::string &value,
                  TreeType &node) const {
    // WARNING: the value is assumed to have been unescaped.
    boost::tuples::get<N>(node.label())[name] = value;
  }
};

// Defines a 'missing' callback for use with DefaultNonTerminalLabeller.
// Assumes the type of TreeType is SyntaxTree<boost::tuple<...> > and that
// the attribute corresponds to the the tuple element N, which is of type
// ElementType.  The callback is configured with a default value which is
// assigned to the tuple element on invocation.
template<typename TreeType, int N, typename ElementType>
class TupleTreeDefaultAssigner {
 public:
  TupleTreeDefaultAssigner(const ElementType &v) : default_value_(v) {}

  void operator()(const std::string &name, TreeType &node) const {
    boost::tuples::get<N>(node.label()) = default_value_;
  }

 private:
  ElementType default_value_;
};

// TODO WRITEME
template<typename TreeType, int N>
class TupleTreeWordAssigner {
 public:
  void operator()(const std::string &word, const SyntaxNode &,
                  TreeType &node) const {
    boost::tuples::get<N>(node.label()) = word;
  }
};

template<typename TreeType, int N, typename ElementType>
class TupleTreeInheritor {
 public:
  // FIXME This assumes that node's parent has been labelled.  Is this
  // guaranteed?
  void operator()(const std::string &word, const SyntaxNode &,
                  TreeType &node) const {
    const TreeType *parent = node.parent();
    assert(parent);
    ElementType parent_val = boost::tuples::get<N>(parent->label());
    boost::tuples::get<N>(node.label()) = parent_val;
  }
};

template<typename T>
DefaultNonTerminalLabeller<T>::DefaultNonTerminalLabeller()
    : default_cb_(NullDefaultCallback) {
}

template<typename T>
void DefaultNonTerminalLabeller<T>::RegisterMatchCallback(
    const std::string &attr_name, const MatchCallbackType &cb) {
  match_cb_map_[attr_name] = cb;
}

template<typename T>
void DefaultNonTerminalLabeller<T>::RegisterMatchCallback(
    const MatchCallbackType &cb) {
  default_cb_ = cb;
}

template<typename T>
void DefaultNonTerminalLabeller<T>::RegisterMissingCallback(
    const std::string &attr_name, const MissingCallbackType &cb) {
  missing_cb_map_[attr_name] = cb;
}

template<typename T>
void DefaultNonTerminalLabeller<T>::Label(const SyntaxNode &tree, T &node) {
  typename MatchMapType::const_iterator p = match_cb_map_.begin();
  typename MatchMapType::const_iterator p_end = match_cb_map_.end();
  SyntaxNode::AttributeSeqType::const_iterator q = tree.m_attributes.begin();
  SyntaxNode::AttributeSeqType::const_iterator q_end = tree.m_attributes.end();

  while (p != p_end || q != q_end) {
    if (p == p_end) {
      // There are no more callbacks.
      default_cb_(q->first, q->second, node);
      ++q;
    } else if (q == q_end) {
      // There are no more XML name-value pairs to process.
      typename MissingMapType::const_iterator r;
      r = missing_cb_map_.find(p->first);
      if (r != missing_cb_map_.end()) {
        r->second(p->first, node);
      }
      ++p;
    } else {
      const std::string &p_name = p->first;
      const std::string &q_name = q->first;
      if (p_name == q_name) {
        // The XML name-value pair has a dedicated callback (p->second).
        p->second(q->first, q->second, node);
        ++p;
        ++q;
      } else if (p_name < q_name) {
        // There is a dedicated callback for p_name but no XML name-value pair.
        typename MissingMapType::const_iterator r;
        r = missing_cb_map_.find(p->first);
        if (r != missing_cb_map_.end()) {
          r->second(p->first, node);
        }
        ++p;
      } else {
        // There is an XML name-value pair without a dedicated callback.
        default_cb_(q->first, q->second, node);
        ++q;
      }
    }
  }
}

template<typename T>
void DefaultTerminalLabeller<T>::Label(const std::string &word,
                                       const SyntaxNode &parent,
                                       T &node) {
  typename CallbackSeqType::const_iterator p = callback_seq_.begin();
  typename CallbackSeqType::const_iterator p_end = callback_seq_.end();
  for (; p != p_end; ++p) {
    (*p)(word, parent, node);
  }
}

template<typename T>
void DefaultTerminalLabeller<T>::RegisterTerminalCallback(
    const CallbackType &cb) {
  callback_seq_.push_back(cb);
}

}  // namespace moses
}  // namespace tool
}  // namespace taco

#endif
