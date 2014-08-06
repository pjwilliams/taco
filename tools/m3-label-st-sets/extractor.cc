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
    if (IsNoun(*preterminal) || IsModifier(*preterminal) ||
        IsPos(*preterminal, "VbMn") || IsPos(*preterminal, "PnDm") ||
        IsPos(*preterminal, "PnRe")) {
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

  // PnRe rule
  if (IsPos(t, "PnRe")) {
    PnReRule(t, warnings);
    return;
  }

  // PnDm rule (second part)
  if (IsPos(t, "PnDm") && IsSubject(*t.parent())) {
    PnDmRuleB(t, warnings);
    return;
  }

  // Check if this is a noun.
  if (!IsNoun(t)) {
    return;
  }

  Tree *noun = &t;

  // Look for the closest sibling to the left.
  Tree *left_sibling = ClosestLeftSibling(t);
  if (left_sibling && IsNoun(*left_sibling)) {
    // If the sibling is also a noun then ignore this one.  Any modifiers
    // to the left will agree with that noun, not this one.  A relation will
    // be extracted for that noun during another call to Extract() (if it
    // hasn't already).
    return;
  }
  if (left_sibling &&
      (Dominates(*left_sibling, "NoCm") || Dominates(*left_sibling, "NoPr"))) {
    // If the sibling dominates a noun then continue but using that node
    // instead of this one.
    noun = left_sibling;
  }

  // Look for AtId / AtDf / AsPpPa / PnId / Aj siblings to the left of the noun.

  std::vector<Tree *> det;
  std::vector<Tree *> non_det;

  while (left_sibling) {
    if (Dominates(*left_sibling, "AtId")) {
      det.push_back(left_sibling);
      break;
    } else if (Dominates(*left_sibling, "AtDf")) {
      det.push_back(left_sibling);
      break;
    } else if (Dominates(*left_sibling, "AsPpPa")) {
      det.push_back(left_sibling);
      break;
    } else if (Dominates(*left_sibling, "NmCd")) {
      det.push_back(left_sibling);
    } else if (Dominates(*left_sibling, "PnId")) {
      non_det.push_back(left_sibling);
    } else if (Dominates(*left_sibling, "Aj")) {
      non_det.push_back(left_sibling);
    } else if (Dominates(*left_sibling, "NmOd")) {
      non_det.push_back(left_sibling);
    }
    left_sibling = ClosestLeftSibling(*left_sibling);
  }

  // Look for PnDm
  Tree *pn_dm = 0;
  const std::vector<Tree *> &siblings = t.parent()->children();
  for (std::size_t i = 0; i < siblings.size(); ++i) {
    Tree *sibling = siblings[i];
    if (IsCat(*sibling, "Atr") && Dominates(*sibling, "PnDm")) {
      pn_dm = sibling;
    }
  }

  // If parent is Sb then look for verb
  Tree *vb_mn = 0;
  if (IsSubject(*t.parent())) {
    if (Tree *right_sibling = ClosestRightSibling(*t.parent())) {
      if (IsPos(*right_sibling, "VbMn")) {
        vb_mn = right_sibling;
      } else if ((right_sibling = ClosestRightSibling(*right_sibling))) {
        if (IsPos(*right_sibling, "VbMn")) {
          vb_mn = right_sibling;
        }
      }
    }
  }

  Tree *as_pp_pa = 0;
  // AsPpPa rule.
  if (det.empty()) {
    Tree *left_grand_sibling = ClosestLeftSibling(*noun->parent());
    if (left_grand_sibling && IsPos(*left_grand_sibling, "AsPpPa")) {
      as_pp_pa = left_grand_sibling;
    }
  }

  // If we've found nothing but a noun then do nothing.
  if (det.empty() && non_det.empty() && !pn_dm && !vb_mn && !as_pp_pa) {
    return;
  }

  // Create a new relation.
  boost::shared_ptr<Relation> r(new Relation());
  relation_vec_.push_back(r);

  // Add the noun.
  if (IsNoun(*noun)) {
    AddNodeToRelation(*noun, *r);
    AddNodeToRelation(*(noun->GetChild(0)), *r);
  } else if (Dominates(*noun, "NoCm")) {
    AddSubtreeToRelation(*noun, "NoCm", *r);
  } else if (Dominates(*noun, "NoPr")) {
    AddSubtreeToRelation(*noun, "NoPr", *r);
  }

  // Add the parent node.
  AddNodeToRelation(*(t.parent()), *r);

  // Add modifiers other than determiners.
  for (std::vector<Tree *>::const_iterator p = non_det.begin();
       p != non_det.end(); ++p) {
    if (Dominates(**p, "PnId")) {
      AddSubtreeToRelation(**p, "PnId", *r);
    } else if (Dominates(**p, "Aj")) {
      AddSubtreeToRelation(**p, "Aj", *r);
    } else if (Dominates(**p, "NmOd")) {
      AddSubtreeToRelation(**p, "NmOd", *r);
    }
  }

  // Add determiners (if any).
  for (std::vector<Tree *>::const_iterator p = det.begin();
       p != det.end(); ++p) {
    if (Dominates(**p, "AtId")) {
      AddSubtreeToRelation(**p, "AtId", *r);
    } else if (Dominates(**p, "AtDf")) {
      AddSubtreeToRelation(**p, "AtDf", *r);
    } else if (Dominates(**p, "AsPpPa")) {
      AddSubtreeToRelation(**p, "AsPpPa", *r);
    } else if (Dominates(**p, "NmCd")) {
      AddSubtreeToRelation(**p, "NmCd", *r);
    }
  }

  // Add PnDm (if any).
  if (pn_dm) {
    AddSubtreeToRelation(*pn_dm, "PnDm", *r);
  }

  // Add AsPpPa (if any).
  if (as_pp_pa) {
    AddNodeToRelation(*as_pp_pa, *r);
    AddNodeToRelation(*(as_pp_pa->GetChild(0)), *r);
  }

  // Add the VbMn if any
  if (vb_mn) {
    AddNodeToRelation(*vb_mn, *r);
    AddNodeToRelation(*(vb_mn->GetChild(0)), *r);
  }
}

void Extractor::PnReRule(Tree &t, std::vector<std::string> &warnings) {
  // Create a new relation.
  boost::shared_ptr<Relation> r(new Relation());
  relation_vec_.push_back(r);

  // Add the PnRe.
  AddNodeToRelation(t, *r);
  AddNodeToRelation(*(t.GetChild(0)), *r);

  // Look for the closest sibling to the left.
  Tree *left_sibling = ClosestLeftSibling(t);
  if (left_sibling && Dominates(*left_sibling, "AtDf")) {
    AddSubtreeToRelation(*left_sibling, "AtDf", *r);
  }

  // Look for the closest sibling to the left of parent.
  Tree *left_grand_sibling = ClosestLeftSibling(*t.parent());
  if (left_grand_sibling && IsPos(*left_grand_sibling, "AsPpPa")) {
    AddNodeToRelation(*t.parent(), *r);
    AddNodeToRelation(*left_grand_sibling, *r);
    AddNodeToRelation(*(left_grand_sibling->GetChild(0)), *r);
  }
}

void Extractor::PnDmRuleB(Tree &t, std::vector<std::string> &warnings) {
  // Create a new relation.
  boost::shared_ptr<Relation> r(new Relation());
  relation_vec_.push_back(r);

  // Add the PnDm.
  AddNodeToRelation(t, *r);
  AddNodeToRelation(*(t.GetChild(0)), *r);

  // Add the Sb
  AddNodeToRelation(*t.parent(), *r);

  // Look for the closest sibling to the right.
  Tree *right_sibling = ClosestRightSibling(*t.parent());
  if (right_sibling) {
    if (IsPos(*right_sibling, "VbMn")) {
      AddNodeToRelation(*right_sibling, *r);
      AddNodeToRelation(*(right_sibling->GetChild(0)), *r);
    } else if ((right_sibling = ClosestRightSibling(*right_sibling))) {
      AddNodeToRelation(*right_sibling, *r);
      AddNodeToRelation(*(right_sibling->GetChild(0)), *r);
    }
  }
}

bool Extractor::IsAj(const Tree &t) {
  return t.IsPreterminal() && t.label().get<kIdxCat>() == "Aj";
}

bool Extractor::IsNoun(const Tree &t) {
  if (!t.IsPreterminal()) {
    return false;
  }
  const std::string &cat = t.label().get<kIdxCat>();
  return cat == "NoCm" || cat == "NoPr";
}

bool Extractor::IsModifier(const Tree &t) {
  if (!t.IsPreterminal()) {
    return false;
  }
  const std::string &cat = t.label().get<kIdxCat>();
  return cat == "Aj" || cat == "AtDf" || cat == "AtId" || cat == "AsPpPa" ||
         cat == "NmCd" || cat == "PnId";
}

bool Extractor::IsSubject(const Tree &t) {
  if (t.IsLeaf() || t.IsPreterminal()) {
    return false;
  }
  return t.label().get<kIdxCat>() == "Sb";
}

bool Extractor::Dominates(const Tree &t, const std::string &pos) {
  if (t.IsLeaf() || t.IsPreterminal()) {
    return false;
  }
  for (std::vector<Tree *>::const_iterator p = t.children().begin();
       p != t.children().end(); ++p) {
    if (IsPos(**p, pos)) {
      return true;
    }
  }
  return false;
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

// Return a pointer to the sibling immediately to the right of t or 0 if none.
Tree *Extractor::ClosestRightSibling(const Tree &t) {
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
  // Is t the rightmost sibling?
  if (i == siblings.size()-1) {
    return 0;
  }
  // Return a pointer to the next sibling.
  return siblings[i+1];
}

void Extractor::AddNodeToRelation(Tree &t, Relation &r) {
  r.nodes.insert(&t);
  node_to_relation_[&t] = &r;
}

void Extractor::AddSubtreeToRelation(Tree &t, const std::string &pos,
                                     Relation &r) {
  AddNodeToRelation(t, r);
  for (std::vector<Tree *>::const_iterator p = t.children().begin();
       p != t.children().end(); ++p) {
    Tree &child = **p;
    if (IsPos(child, pos)) {
      AddNodeToRelation(child, r);
      AddNodeToRelation(*(child.GetChild(0)), r);
    }
  }
}

bool Extractor::IsPos(const Tree &t, const std::string &tag) {
  return t.IsPreterminal() && t.label().get<kIdxCat>() == tag;
}

bool Extractor::IsCat(const Tree &t, const std::string &tag) {
  return t.label().get<kIdxCat>() == tag;
}

}  // namespace m3
}  // namespace tool
}  // namespace taco
