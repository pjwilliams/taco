#include "extractor.h"

#include <cassert>
#include <map>
#include <vector>

#include "taco/base/exception.h"

#include "tools-common/relation/relation.h"

namespace taco {
namespace tool {
namespace m3 {

void Extractor::Extract(Tree &t, std::set<Relation> &relations,
                        std::vector<std::string> &warnings) {
  relation_vec_.clear();
  node_to_relation_.clear();

  // Visit nodes extracting noun-modifier agreement relations.
  for (std::vector<Tree *>::const_iterator p = t.children().begin();
       p != t.children().end(); ++p) {
    Extract(**p, warnings);
  }

  // Add relations to output set.
  for (RelationVec::const_iterator p = relation_vec_.begin();
       p != relation_vec_.end(); ++p) {
    relations.insert(**p);
  }

  // Mop up any unclaimed modifiers and nouns to produce single-word
  // selector-target sets.  Whilst those selectors / targets don't participate
  // in an agreement relation in this particular parse tree, the rules they
  // end up in may become part of a derivation where there is such a relation.
  std::vector<Tree *> leaves;
  t.GetLeaves(leaves);
  for (std::vector<Tree *>::const_iterator p = leaves.begin();
       p != leaves.end(); ++p) {
    Tree *leaf = *p;
    // Already in a relation?
    if (node_to_relation_.find(leaf) != node_to_relation_.end()) {
      continue;
    }
    Tree *preterminal = leaf->parent();
    if (IsNoCm(*preterminal) || IsModifier(*preterminal)) {
      Relation r;
      AddNodeToRelation(*preterminal, r);
      AddNodeToRelation(*leaf, r);
      relations.insert(r);
    }
  }
}

void Extractor::Extract(Tree &t, std::vector<std::string> &warnings) {
  // Recursively visit children first.
  for (std::vector<Tree *>::const_iterator p = t.children().begin();
       p != t.children().end(); ++p) {
    Extract(**p, warnings);
  }

  // Check if this is a common noun.
  if (!IsNoCm(t)) {
    return;
  }

  // Look for the closest sibling to the left.
  Tree *left_sibling = ClosestLeftSibling(t);
  if (!left_sibling) {
    return;
  }

  // If the sibling is also a common noun then ignore this noun.  Any modifiers
  // to the left will agree with that noun, not this one.
  if (IsNoCm(*left_sibling)) {
    return;
  }

  // Create a new relation.
  boost::shared_ptr<Relation> r(new Relation());
  relation_vec_.push_back(r);

  // Add the noun.
  AddNodeToRelation(t, *r);
  AddNodeToRelation(*(t.GetChild(0)), *r);

  // Add the parent node.
  AddNodeToRelation(*(t.parent()), *r);

  // Add any Det / Aj siblings to the relation, terminating after the first Det.
  while (true) {
    if (IsDet(*left_sibling)) {
      try {
        AddUnaryChainToRelation(*left_sibling, *r);
      } catch(const Exception &e) {
        std::ostringstream msg;
        msg << "failed to add Det chain to relation: " << e.msg();
        warnings.push_back(msg.str());
      }
      break;
    }
    if (DominatesAj(*left_sibling)) {
      AddAjSubtreeToRelation(*left_sibling, *r);
    }
    left_sibling = ClosestLeftSibling(*left_sibling);
    if (!left_sibling) {
      break;
    }
  }
}

bool Extractor::IsAj(const Tree &t) {
  return t.IsPreterminal() && t.label().get<kIdxCat>() == "Aj";
}

bool Extractor::IsNoCm(const Tree &t) {
  return t.IsPreterminal() && t.label().get<kIdxCat>() == "NoCm";
}

bool Extractor::IsModifier(const Tree &t) {
  if (!t.IsPreterminal()) {
    return false;
  }
  const std::string &cat = t.label().get<kIdxCat>();
  return cat == "Aj" || cat == "AtDf" || cat == "AtId";
}

bool Extractor::IsDet(const Tree &t) {
  if (t.IsLeaf() || t.IsPreterminal()) {
    return false;
  }
  return t.label().get<kIdxCat>() == "Det";
}

bool Extractor::DominatesAj(const Tree &t) {
  if (t.IsLeaf() || t.IsPreterminal()) {
    return false;
  }
  const std::vector<Tree *> &children = t.children();
  return children.size() == 1 && IsAj(*t.GetChild(0));
}

// Return a pointer to the sibling immediately to the left of t or 0 if none.
Tree *Extractor::ClosestLeftSibling(const Tree &t) {
  const Tree *parent = t.parent();
  if (!parent) {
    return 0;
  }
  // Find t's position among its siblings.
  const std::vector<Tree *> &siblings = parent->children();
  std::size_t i = 0;
  for (; i < siblings.size(); ++i) {
    if (siblings[i] == &t) {
      break;
    }
  }
  // Is t the leftmost sibling?
  if (i == 0) {
    return 0;
  }
  // Return a pointer to the previous sibling.
  return siblings[i-1];
}

void Extractor::AddNodeToRelation(Tree &t, Relation &r) {
  r.nodes.insert(&t);
  node_to_relation_[&t] = &r;
}

void Extractor::AddUnaryChainToRelation(Tree &t, Relation &r) {
  const std::vector<Tree *> &children = t.children();
  if (children.size() > 1) {
    throw Exception("unary chain is not unary");
  }
  if (children.size() == 1) {
    AddUnaryChainToRelation(*children[0], r);
  }
  r.nodes.insert(&t);
  node_to_relation_[&t] = &r;
}

void Extractor::AddAjSubtreeToRelation(Tree &t, Relation &r) {
  AddNodeToRelation(t, r);
  for (std::vector<Tree *>::const_iterator p = t.children().begin();
       p != t.children().end(); ++p) {
    Tree *child = *p;
    if (IsAj(*child)) {
      AddNodeToRelation(*child, r);
      AddNodeToRelation(*(child->GetChild(0)), r);
    }
  }
}

}  // namespace m3
}  // namespace tool
}  // namespace taco
