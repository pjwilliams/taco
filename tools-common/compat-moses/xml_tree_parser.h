#ifndef TACO_TOOLS_COMMON_COMPAT_MOSES_XML_TREE_PARSER_H_
#define TACO_TOOLS_COMMON_COMPAT_MOSES_XML_TREE_PARSER_H_

#include "xml_tree_parser_defaults.h"

#include "taco/base/exception.h"

#include "tools-common/compat-moses/syntax_tree.h"
#include "tools-common/compat-moses/tables_core.h"
#include "tools-common/compat-moses/xml_exception.h"
#include "tools-common/compat-moses/xml_tree.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace taco {
namespace tool {
namespace moses {

// Synopsis: parses a string in Moses' XML parse tree format and returns a tree
// of type T, which should be a taco::SyntaxTree or else provide a compatible
// interface.
//
// High-level description of XmlTreeParser algorithm:
//
// 1. Uses Moses code to read a line into vector of word strings and a
//    Moses::SyntaxTree.
//
// 2. Builds a fully-connected SyntaxTree by creating one SyntaxTree for each
//    Moses::SyntaxTree and then attaching a SyntaxTree leaf for each word.
//
// 3. Labels the tree by calling a NonTerminalLabeller policy function on each
//    non-terminal and a TerminalLabeller function on each terminal.  Labelling
//    functions are invoked in an arbitrary order.  The NonTerminalLabller and
//    TerminalLabeller are policy classes.  By default, the classes are
//    DefaultNonTerminalLabeller and DefaultTerminalLabeller, respectively.
//    See xml_tree_parser_defaults.h.
//
// Steps 2 and 3 may actually be performed as a single step.  A labelling
// policy must assume nothing about the connectedness of the partially-
// constructed SyntaxTree nor about the labels that might have been set on
// other nodes.  It does however have access to the full Moses::SyntaxTree.
//
// FIXME Provide guarantees about labelling order?
//
// Informal description of assumed Moses XML tree format:
//
// A sequence of lines, each consisting of a sequence of XML tags and words
// that together represent a sentence and a corresponding phrase-structure tree.
//
// The sentence is a sequence of words.  The words are not attached directly to
// the tree, but the tree either contains span information or encloses the words
// to which it is associated.
//
// Each tree node has a sequence of name-value attributes.  The "label"
// attribute is the only attribute guaranteed to be present.  Its value is a
// string representing a non-terminal label.  It does not contain brackets
// (unlike the [X] format used in extract files, etc.)
//
// TODO Comment about thread safety.
template<typename T,
         typename NL=DefaultNonTerminalLabeller<T>,
         typename TL=DefaultTerminalLabeller<T> >
class XmlTreeParser : public NL, public TL {
 public:
  typedef NL NonTerminalLabeller;
  typedef TL TerminalLabeller;

  XmlTreeParser();

  std::auto_ptr<T> Parse(const std::string &);

 private:
  std::auto_ptr<T> ConvertTree(SyntaxNode &,
                               const std::vector<std::string> &);

  void CreateLeaf(SyntaxNode &, const std::string &, T &);

  // TODO Provide the client with a means of accessing label_set_ and
  // top_label_set_ (or passing in its own).
  std::set<std::string> label_set_;
  std::map<std::string, int> top_label_set_;
  std::string line_;
  SyntaxTree tree_;
  std::vector<std::string> words_;
};

template<typename T, typename NL, typename TL>
XmlTreeParser<T,NL,TL>::XmlTreeParser() {
}

template<typename T, typename NL, typename TL>
std::auto_ptr<T> XmlTreeParser<T,NL,TL>::Parse(const std::string &line) {
  line_ = line;
  tree_.Clear();
  try {
    if (!ProcessAndStripXMLTags(line_, tree_, label_set_, top_label_set_)) {
      throw Exception("");
    }
  } catch (const XmlException &e) {
    throw Exception(e.getMsg());
  }
  if (tree_.GetNumWords() == 0) {
    return std::auto_ptr<T>();
  }
  tree_.ConnectNodes();
  SyntaxNode *root = tree_.GetTop();
  assert(root);
  words_ = tokenize(line_.c_str());
  return ConvertTree(*root, words_);
}

// Converts a SyntaxNode tree to a tree of type T.  The input tree doesn't
// include words whereas in the output tree they are attached as leaves.
// Otherwise, the input and output tree structures are isomorphic.
template<typename T, typename NL, typename TL>
std::auto_ptr<T> XmlTreeParser<T,NL,TL>::ConvertTree(
    SyntaxNode &in_root,
    const std::vector<std::string> &words) {
  // Create the tree node.
  std::auto_ptr<T> out_root(new T());

  // Handle attributes.
  std::sort(in_root.m_attributes.begin(), in_root.m_attributes.end());
  NonTerminalLabeller::Label(in_root, *out_root);

  // Create children.  In addition to recursively converting the children of
  // 'in_root', leaf nodes must be created for any words that are to be attached
  // directly to the tree.  The original SyntaxNode-based tree doesn't include
  // the words so the attachment points are determined from the spans.
  int i = in_root.GetStart();
  const std::vector<SyntaxNode*> &children = in_root.GetChildren();
  for (std::vector<SyntaxNode*>::const_iterator p = children.begin();
       p != children.end(); ++p) {
    assert(*p);
    SyntaxNode &child = **p;

    // Create leaves for any words that come to the left of the first child.
    while (i < child.GetStart()) {
      CreateLeaf(in_root, words[i++], *out_root);
    }
    // TODO Connect child to parent before attribute handlers are called.
    // This will allow handlers to, for example, inherit attribute values
    // from ancestors.  Currently this is only possible for leaf nodes.
    std::auto_ptr<T> dst_child = ConvertTree(child, words);
    dst_child->parent() = out_root.get();
    out_root->AddChild(dst_child.release());
    i = child.GetEnd()+1;
  }
  // Create leaves for any words that come to the right of the last child.
  while (i <= in_root.GetEnd()) {
    CreateLeaf(in_root, words[i++], *out_root);
  }

  return out_root;
}

template<typename T, typename NL, typename TL>
void XmlTreeParser<T,NL,TL>::CreateLeaf(SyntaxNode &tree,
                                        const std::string &word,
                                        T &parent) {
  std::auto_ptr<T> leaf(new T());
  leaf->parent() = &parent;
  parent.AddChild(leaf.get());
  TerminalLabeller::Label(word, tree, *leaf);
  leaf.release();
}

}  // namespace moses
}  // namespace tool
}  // namespace taco

#endif
