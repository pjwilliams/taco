#include "extractor_bitpar.h"

#include "tools-common/compat-nlp-de/bitpar.h"
#include "tools-common/relation/relation.h"

#include "taco/base/exception.h"

#include <cassert>
#include <map>
#include <vector>

namespace taco {
namespace tool {
namespace m1 {

// TODO Multiple subjects (See sentence 87)

void ExtractorBitPar::Extract(Tree &t, std::set<Relation> &relations,
                        std::vector<std::string> &warnings) {
  relation_vec_.clear();
  node_to_relation_.clear();

  label_parser_.Parse(t.label().get<kIdxCat>(), label_);
  if (label_.cat != "TOP") {
    throw Exception("root label is not \"TOP\" or \"TOP-*\"");
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

void ExtractorBitPar::ExtractNpPp(Tree &t, std::vector<std::string> &warnings) {
  if (t.IsPreterminal() || t.IsLeaf()) {
    return;
  }
  // Check if this node already belongs to a relation (created by an ancestor
  // node).
  if (node_to_relation_.find(&t) == node_to_relation_.end()) {
    label_parser_.Parse(t.label().get<kIdxCat>(), label_);
    if (label_.cat == "NP" || label_.cat == "PP") {
      boost::shared_ptr<Relation> r(new Relation());
      AddNpPpToRelation(t, *r);
      if (!r->nodes.empty()) {
        relation_vec_.push_back(r);
      }
    }
    // TODO CNP / CPP
  }
  // Visit children.
  for (std::vector<Tree *>::const_iterator p = t.children().begin();
       p != t.children().end(); ++p) {
    ExtractNpPp(**p, warnings);
  }
}

void ExtractorBitPar::ExtractSubjVerb(Tree &t, std::vector<std::string> &warnings) {
  if (t.IsPreterminal() || t.IsLeaf()) {
    return;
  }
  label_parser_.Parse(t.label().get<kIdxCat>(), label_);
  if (label_.cat == "S") {
    AddClauseToRelation(t);
  }
  // Visit children.
  for (std::vector<Tree *>::const_iterator p = t.children().begin();
       p != t.children().end(); ++p) {
    ExtractSubjVerb(**p, warnings);
  }
}

void ExtractorBitPar::AddClauseToRelation(Tree &t) {
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

void ExtractorBitPar::AddNpPpToRelation(Tree &t, Relation &r) {
                                //std::vector<std::string> &warnings) {
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
      break;
    }
  }
}

Tree *ExtractorBitPar::FindNounPhraseHead(Tree &t) {
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

bool ExtractorBitPar::IsComma(const Tree &t) {
  if (!t.IsPreterminal()) {
    return false;
  }
  const std::string &label = t.label().get<kIdxCat>();
  label_parser_.Parse(label, label_);
  return label_.cat == "PUNC,";
}

bool ExtractorBitPar::IsNounOrPronoun(const Tree &t) {
  if (!t.IsPreterminal()) {
    return false;
  }
  const std::string &label = t.label().get<kIdxCat>();
  label_parser_.Parse(label, label_);
  return label_.cat == "NE" ||
         label_.cat == "NN" ||
         label_.cat == "PPER" ||
         label_.cat == "PDS";
}

bool ExtractorBitPar::IsProperNoun(const Tree &t) {
  if (!t.IsPreterminal()) {
    return false;
  }
  const std::string &label = t.label().get<kIdxCat>();
  label_parser_.Parse(label, label_);
  return label_.cat == "NE";
}

bool ExtractorBitPar::IsPreposition(const Tree &t) {
  if (!t.IsPreterminal()) {
    return false;
  }
  const std::string &label = t.label().get<kIdxCat>();
  label_parser_.Parse(label, label_);
  return label_.cat == "APPR" || label_.cat == "APPRART";
}

bool ExtractorBitPar::IsModifier(const Tree &t) {
  if (!t.IsPreterminal()) {
    return false;
  }
  const std::string &label = t.label().get<kIdxCat>();
  label_parser_.Parse(label, label_);
  return label_.cat == "ADJA" ||
         label_.cat == "ART" ||
         label_.cat == "PDAT" ||
         label_.cat == "PIAT" ||
         label_.cat == "PPOSAT" ||
         label_.cat == "PWAT";
}

bool ExtractorBitPar::IsSubject(const Tree &t) {
  if (t.IsPreterminal() || t.IsLeaf()) {
    return false;
  }
  const std::string &label = t.label().get<kIdxCat>();
  label_parser_.Parse(label, label_);
  return label_.func == "EP" || label_.func == "SB";
}

bool ExtractorBitPar::IsFiniteVerb(const Tree &t) {
  if (!t.IsPreterminal()) {
    return false;
  }
  const std::string &label = t.label().get<kIdxCat>();
  label_parser_.Parse(label, label_);
  return label_.cat == "VAFIN" || label_.cat == "VMFIN" ||
         label_.cat == "VVFIN";
}

void ExtractorBitPar::AddWordToRelation(Tree &t, Relation &r) {
  assert(t.IsPreterminal());
  AddNodeToRelation(t, r);
  AddNodeToRelation(*(t.GetChild(0)), r);
}

void ExtractorBitPar::AddNodeToRelation(Tree &t, Relation &r) {
  r.nodes.insert(&t);
  node_to_relation_[&t] = &r;
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
