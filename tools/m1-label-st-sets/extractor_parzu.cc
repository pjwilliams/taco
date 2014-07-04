#include "extractor_parzu.h"

#include "tools-common/relation/relation.h"

#include "taco/base/exception.h"

#include <cassert>
#include <map>
#include <vector>

namespace taco {
namespace tool {
namespace m1 {

void ExtractorParZu::Extract(Tree &t, std::set<Relation> &relations,
                        std::vector<std::string> &warnings) {
  relation_vec_.clear();
  node_to_relation_.clear();

  if (t.label().get<kIdxCat>() != "sent") {
    throw Exception("root label is not \"sent\"");
  }

  // Recursively visit nodes extracting NP/PP relations.
  for (std::vector<Tree *>::const_iterator p = t.children().begin();
       p != t.children().end(); ++p) {
    ExtractNpPp(**p, warnings);
  }

  // Optionally extract subject-verb relations.
  if (options_.subject_verb) {
    for (std::vector<Tree *>::const_iterator p = t.children().begin();
         p != t.children().end(); ++p) {
      ExtractSubjVerb(**p, warnings);
    }
  }

  // Add complete relations to set.
  for (RelationVec::const_iterator p = relation_vec_.begin();
       p != relation_vec_.end(); ++p) {
    relations.insert(**p);
  }

  // Mop up unclaimed prepositions, modifiers, nouns, and finite verbs.
  std::vector<Tree *> leaves;
  t.GetLeaves(leaves);
  for (std::vector<Tree *>::const_iterator p = leaves.begin();
       p != leaves.end(); ++p) {
    Tree *leaf = *p;
    if (node_to_relation_.find(leaf) != node_to_relation_.end()) {
      continue;
    }
    Tree *preterminal = leaf->parent();
    if (IsPreposition(*preterminal) ||
        IsModifier(*preterminal) ||
        (IsNounOrPronoun(*preterminal) && !IsProperNoun(*preterminal)) ||
        IsFiniteVerb(*preterminal)) {
      Relation r;
      AddWordToRelation(*preterminal, r);
      relations.insert(r);
    }
  }
}

void ExtractorParZu::ExtractNpPp(Tree &t, std::vector<std::string> &warnings) {
  if (t.IsPreterminal() || t.IsLeaf()) {
    return;
  }
  // Check if this node already belongs to a relation (created by an ancestor
  // node).
  if (node_to_relation_.find(&t) == node_to_relation_.end()) {
    const std::string &cat = t.label().get<kIdxCat>();
    if (cat == "subj" || cat == "obja" || cat == "objd" || cat == "objg" ||
        cat == "pred" || cat == "gmod" || cat == "gmod_pre" ||
        cat == "gmod_post" || cat == "zeit" || cat == "app" || cat == "pp" ||
        cat == "objp" || cat == "root" || cat == "kon_root"  ||
        cat == "kon_gmod"  || cat == "kon_gmod_post"  ||
        cat == "kon_gmod_pre" || cat == "kon_obja"  || cat == "kon_objd"  ||
        cat == "kon_objg"  || cat == "kon_pp" || cat == "kon_objp" ||
        cat == "kon_subj" || cat == "kon_pred" || cat == "kon_zeit") {
      boost::shared_ptr<Relation> r(new Relation());
      AddNpPpToRelation(t, *r);
      if (!r->nodes.empty()) {
        relation_vec_.push_back(r);
      }
    }
  }
  // Visit children.
  for (std::vector<Tree *>::const_iterator p = t.children().begin();
       p != t.children().end(); ++p) {
    ExtractNpPp(**p, warnings);
  }
}

void ExtractorParZu::ExtractSubjVerb(Tree &t,
                                     std::vector<std::string> &warnings) {
  if (t.IsPreterminal() || t.IsLeaf()) {
    return;
  }
  const std::string &cat = t.label().get<kIdxCat>();
  if (cat == "kon" || cat == "root" || cat == "vroot" || cat == "neb" ||
      cat == "rel" || cat == "s" || cat == "subjc" || cat == "objc" ||
      cat == "kon_vroot") {
    AddClauseToRelation(t);
  }
  // Visit children.
  for (std::vector<Tree *>::const_iterator p = t.children().begin();
       p != t.children().end(); ++p) {
    ExtractSubjVerb(**p, warnings);
  }
}

void ExtractorParZu::AddClauseToRelation(Tree &t) {
  Tree *subj = 0;
  Tree *verb = 0;
  for (std::vector<Tree *>::const_iterator p = t.children().begin();
       p != t.children().end(); ++p) {
    Tree &child = **p;
    if (IsSubject(child)) {
      // TODO warn if multiple subjects
      subj = &child;
    } else if (IsFiniteVerb(child)) {
      verb = &child;
    }
  }
  if (subj && verb) {
    Relation *r;
    NodeToRelationMap::const_iterator p = node_to_relation_.find(subj);
    if (p == node_to_relation_.end()) {
      boost::shared_ptr<Relation> relation(new Relation());
      relation_vec_.push_back(relation);
      r = relation.get();
    } else {
      r = p->second;
    }
    AddNodeToRelation(*subj, *r);
    AddWordToRelation(*verb, *r);
    AddNodeToRelation(t, *r);
  }
}

void ExtractorParZu::AddNpPpToRelation(Tree &t, Relation &r) {
  AddNodeToRelation(t, r);
  Tree *head = FindNounPhraseHead(t);
  Tree *appr = 0;
  for (std::vector<Tree *>::const_iterator p = t.children().begin();
       p != t.children().end(); ++p) {
    Tree &child = **p;
    if (IsPreposition(child)) {
      appr = &child;  // Only include the last preposition
    } else if (IsModifier(child)) {
      if (appr) {
        AddWordToRelation(*appr, r);
        appr = 0;
      }
      AddWordToRelation(child, r);
    } else if (&child == head) {
      if (appr) {
        AddWordToRelation(*appr, r);
        appr = 0;
      }
      if (!IsProperNoun(child)) {
        AddWordToRelation(child, r);
      }
//       break; // coordinated elements may come after head
    }
  }
}

Tree *ExtractorParZu::FindNounPhraseHead(Tree &t) {
  Tree *head = 0;
  for (std::vector<Tree *>::const_iterator p = t.children().begin();
       p != t.children().end(); ++p) {
    Tree &child = **p;
    if (head) {
      if (IsNounOrPronoun(child)) {
        head = &child;
      } else {
        return head;
      }
    } else if (IsNounOrPronoun(child)) {
      head = &child;
    } else if (IsComma(child)) {
      break;
    }
  }
  return head;
}

bool ExtractorParZu::IsComma(const Tree &t) {
  return t.IsPreterminal() && t.label().get<kIdxCat>() == "$,";
}

bool ExtractorParZu::IsNounOrPronoun(const Tree &t) {
  const std::string &cat = t.label().get<kIdxCat>();
  return cat == "NE" || cat == "NN" || cat == "PPER" || cat == "PDS";
}

bool ExtractorParZu::IsProperNoun(const Tree &t) {
  return t.IsPreterminal() && t.label().get<kIdxCat>() == "NE";
}

bool ExtractorParZu::IsPreposition(const Tree &t) {
  const std::string &cat = t.label().get<kIdxCat>();
  return t.IsPreterminal() && (cat == "APPR" || cat == "APPRART");
}

bool ExtractorParZu::IsModifier(const Tree &t) {
  if (t.IsLeaf()) {
    return false;
  }
  const std::string &cat = t.label().get<kIdxCat>();
  return cat == "ADJA" ||
         cat == "ART" ||
         cat == "PDAT" ||
         cat == "PIAT" ||
         cat == "PIDAT" ||
         cat == "PPOSAT" ||
         cat == "PWAT" ||
         cat == "det" ||
         cat == "pn" ||
         cat == "attr" ||
         cat == "kon_attr" ||
         cat == "kon_pn" ||
         cat == "SEGMENT" ||
         cat == "kon_det";
}

bool ExtractorParZu::IsSubject(const Tree &t) {
  if (t.IsPreterminal() || t.IsLeaf()) {
    return false;
  }
  const std::string &cat = t.label().get<kIdxCat>();
  return cat == "expl" || cat == "subj";
}

bool ExtractorParZu::IsFiniteVerb(const Tree &t) {
  if (!t.IsPreterminal()) {
    return false;
  }
  const std::string &cat = t.label().get<kIdxCat>();
  return cat == "VAFIN" || cat == "VMFIN" || cat == "VVFIN";
}

void ExtractorParZu::AddWordToRelation(Tree &t, Relation &r) {
  AddNodeToRelation(t, r);
  if (t.IsPreterminal()) {
    AddNodeToRelation(*(t.GetChild(0)), r);
  } else {
    AddNpPpToRelation(t, r);
  }
}

void ExtractorParZu::AddNodeToRelation(Tree &t, Relation &r) {
  r.nodes.insert(&t);
  node_to_relation_[&t] = &r;
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
