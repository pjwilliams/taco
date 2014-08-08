#include "extractor.h"

#include "options.h"

#include <cassert>
#include <map>
#include <set>

namespace taco {
namespace tool {
namespace m3 {

Extractor::Extractor(Vocabulary &feature_set, Vocabulary &value_set,
                     const Options &options)
    : feature_set_(feature_set)
    , value_set_(value_set)
    , options_(options) {
  // "CAT"
  cat_path_.push_back(feature_set_.Insert("CAT"));
  // "AGREEMENT"
  agr_path_.push_back(feature_set_.Insert("AGREEMENT"));
  // "AGREEMENT,CASE"
  case_path_.push_back(feature_set_.Insert("AGREEMENT"));
  case_path_.push_back(feature_set_.Insert("CASE"));
}

// Form a map from rule nodes to constraint indices.  The root node has
// constraint index 0 and the leaves have contiguous indices starting at 1
// in the order defined by fragment.leaves.
void Extractor::BuildConstraintIndexMap(const TreeFragment &fragment,
                                        ConstraintIndexMap &map) const {
  map.clear();
  map[fragment.root] = 0;
  for (size_t i = 0; i < fragment.leaves.size(); ++i) {
    map[fragment.leaves[i]] = i+1;
  }
  assert(map.size() == fragment.leaves.size()+1);
}

void Extractor::Extract(const TreeFragment &fragment,
                           const TreeContext &context,
                           CSVec &constraint_sets) {
  typedef std::vector<const Tree *> RelationVec;
  typedef std::map<int, RelationVec> FragmentRelationMap;

  constraint_sets.clear();

  // Build a map from fragment nodes to constraint indices.
  BuildConstraintIndexMap(fragment, constraint_index_map_);

  // Group nodes into sets according to relation ID.
  FragmentRelationMap relations;
  int id = fragment.root->label().get<kIdxId>();
  if (id != -1) {
    relations[id].push_back(fragment.root);
  }
  for (std::vector<Tree *>::const_iterator p = fragment.leaves.begin();
       p != fragment.leaves.end(); ++p) {
    Tree &node = **p;
    int id = node.label().get<kIdxId>();
    if (id != -1) {
      relations[id].push_back(&node);
    }
  }

  // Form a ConstraintSet from each set of nodes.
  for (FragmentRelationMap::const_iterator p = relations.begin();
       p != relations.end(); ++p) {
    const int relation_id = p->first;
    const RelationVec &relation_vec = p->second;

    boost::shared_ptr<ConstraintSet> cs(new ConstraintSet);

    bool purely_lexical = true;

    // Check for non-lexical rule elements.
    {
      RelationVec::const_iterator q = relation_vec.begin();
      for (; q != relation_vec.end(); ++q) {
        const Tree *element = *q;
        if (!element->IsLeaf()) {
          purely_lexical = false;
          break;
        }
      }
    }

    int num_rel_constraints = 0;
    int num_abs_constraints = 0;
    int num_var_constraints = 0;

    // Produce RelConstraints.
    RelationVec::const_iterator q = relation_vec.begin();
    PathTerm lhs(constraint_index_map_[*q], agr_path_);
    for (++q; q != relation_vec.end(); ++q) {
      PathTerm rhs(constraint_index_map_[*q], agr_path_);
      boost::shared_ptr<RelConstraint> rc(new RelConstraint(lhs, rhs));
      cs->rel_set().Insert(rc);
      ++num_rel_constraints;
    }

    // Produce an AbsConstraint for each word.
    if (!options_.no_cat) {
      for (RelationVec::const_iterator q = relation_vec.begin();
           q != relation_vec.end(); ++q) {
        const Tree &node = **q;
        if (!node.IsLeaf()) {
          continue;
        }
        PathTerm lhs(constraint_index_map_[&node], cat_path_);
        const Tree &preterminal = *(node.parent());
        std::string pos = preterminal.label().get<kIdxCat>();
        if (options_.map_cat_values) {
          if (!MapPosTag(pos)) {
            continue;  // Skip constraint if we don't know how to map the tag.
          }
        }
        ValueTerm rhs(value_set_.Insert(pos));
        boost::shared_ptr<AbsConstraint> ac(new AbsConstraint(lhs, rhs));
        cs->abs_set().Insert(ac);
        ++num_abs_constraints;
      }
    }

    typedef m1::CaseTable::ProbabilityFunction ProbabilityFunction;

    // Produce case model constraint, if applicable.
    const CaseModelMap *case_model_map = context.GetCaseModelMap();
    if (case_model_map) {
      CaseModelMap::const_iterator r = case_model_map->find(relation_id);
      if (r != case_model_map->end()) {
        NounPhraseDistPair pair = r->second;
        const Tree *noun_phrase_root = pair.first;
        if (fragment.Contains(noun_phrase_root, false)) {
          const ProbabilityFunction &prob_func = *(pair.second);
          PathTerm lhs(constraint_index_map_[relation_vec[0]], case_path_);
          if (options_.case_model_threshold == -1) {  // Soft constraint
            VarTerm rhs(prob_func);
            boost::shared_ptr<VarConstraint> vc(new VarConstraint(lhs, rhs));
            cs->var_set().Insert(vc);
            ++num_var_constraints;
          } else { // hard constraint
            for (ProbabilityFunction::const_iterator p = prob_func.begin();
                 p != prob_func.end(); ++p) {
              if (p->second < options_.case_model_threshold) {
                continue;
              }
              ValueTerm rhs(p->first);
              boost::shared_ptr<AbsConstraint> ac(new AbsConstraint(lhs, rhs));
              cs->abs_set().Insert(ac);
              ++num_abs_constraints;
              break;
            }
          }
        }
      }
    }

    if ((num_rel_constraints && (!purely_lexical || options_.retain_lexical))
        || num_var_constraints) {
      constraint_sets.push_back(cs);
    }
  }
}

bool Extractor::MapPosTag(std::string &tag) const {
  if (tag == "NoCm" || tag == "NoPr") {
    tag = "noun";
  } else if (tag == "AtDf" || tag == "AtId" || tag == "AsPpPa" ||
             tag == "NmCd") {
    tag = "article";
  } else if (tag == "Aj" || tag == "NmOd") {
    tag = "adjective";
  } else if (tag == "VbMn") {
    tag = "verb";
  } else if (tag == "PnId" || tag == "PnRe" || tag == "PnDm" || tag == "PnPe") {
    tag = "pronoun";
  } else {
    return false;
  }
  return true;
}

}  // namespace m3
}  // namespace tool
}  // namespace taco
