#ifndef TACO_TOOLS_COMMON_M1_RELATION_EVALUATOR_H_
#define TACO_TOOLS_COMMON_M1_RELATION_EVALUATOR_H_

#include "tools-common/compat-nlp-de/bitpar.h"

#include "tools-common/relation/relation.h"
#include "tools-common/relation/relation_tree_ops.h"

#include "taco/constraint.h"
#include "taco/constraint_evaluator.h"
#include "taco/constraint_set.h"
#include "taco/feature_structure.h"
#include "taco/lexicon.h"
#include "taco/base/numbered_set.h"
#include "taco/option_table.h"
#include "taco/base/vocabulary.h"

#include <boost/shared_ptr.hpp>

#include <iterator>
#include <map>
#include <string>
#include <vector>

namespace taco {
namespace tool {
namespace m1 {

// Evaluates selector-target relations defined between nodes of a
// SyntaxTree<...> of type T.  The SyntaxTree label type is assumed to be a
// tuple in which element N is a BitPar label.  The template parameter R
// defines the relation type, which should be either Relation<T*> or
// Relation<const T*>.
//
template<typename T, typename R, int N>
class RelationEvaluator {
 public:
  RelationEvaluator(const Lexicon<std::size_t> &lexicon,
                    const Vocabulary &vocab,
                    const Vocabulary &value_set,
                    Feature infl_feature,
                    Feature cat_feature)
      : lexicon_(lexicon)
      , vocab_(vocab)
      , value_set_(value_set)
      , infl_feature_path_(1, infl_feature)
      , cat_feature_path_(1, cat_feature) {}

  bool Evaluate(const R &);

  bool Evaluate(const R &, std::vector<Interpretation> &);

 private:
  typedef NumberedSet<const T *, short> LeafIndexMap;

  struct EvaluationState {
    void Clear() {
      leaf_index_map.Clear();
      constraint_set.Clear();
      option_table.Clear();
      fs_vec.clear();
    }
    LeafIndexMap leaf_index_map;
    ConstraintSet constraint_set;
    OptionTable option_table;
    std::vector<boost::shared_ptr<FeatureStructure> > fs_vec;
  };

  void BuildLeafIndexMap(const R &);
  void BuildConstraintSet(const R &);
  void BuildOptionTable(const R &);

  const Lexicon<std::size_t> &lexicon_;
  const Vocabulary &vocab_;
  const Vocabulary &value_set_;
  const FeaturePath infl_feature_path_;
  const FeaturePath cat_feature_path_;
  de::BitParLabelParser label_parser_;
  ConstraintEvaluator evaluator_;
  EvaluationState state_;
};

template<typename T, typename R, int N>
bool RelationEvaluator<T,R,N>::Evaluate(const R &relation) {
  assert(relation.Size() > 1);
  BuildLeafIndexMap(relation);
  BuildConstraintSet(relation);
  BuildOptionTable(relation);
  bool result = evaluator_.Eval(state_.option_table, state_.constraint_set);
  state_.Clear();
  return result;
}

template<typename T, typename R, int N>
bool RelationEvaluator<T,R,N>::Evaluate(
    const R &relation,
    std::vector<Interpretation> &interpretations) {
  assert(relation.Size() > 1);
  BuildLeafIndexMap(relation);
  BuildConstraintSet(relation);
  BuildOptionTable(relation);
  bool result = evaluator_.Eval(state_.option_table, state_.constraint_set,
                                interpretations);
  state_.Clear();
  return result;
}

template<typename T, typename R, int N>
void RelationEvaluator<T,R,N>::BuildLeafIndexMap(const R &relation) {
  // Assign a distinct index to each leaf in the relation.  These are used as
  // indices in the constraints and in the option table.
  state_.leaf_index_map.Clear();
  for (typename R::ConstIterator p = relation.Begin();
       p != relation.End(); ++p) {
    const T *tree = *p;
    if (tree->IsLeaf()) {
      state_.leaf_index_map.Insert(tree);
    }
  }
}

template<typename T, typename R, int N>
void RelationEvaluator<T,R,N>::BuildConstraintSet(const R &relation) {
  state_.constraint_set.Clear();
  int lhs_index = -1;
  for (typename R::ConstIterator p = relation.Begin();
       p != relation.End(); ++p) {
    const T *tree = *p;
    if (!tree->IsLeaf()) {
      continue;
    }
    typename LeafIndexMap::IdType index = state_.leaf_index_map.Lookup(tree);
    if (lhs_index == -1) {
      lhs_index = index;
      assert(lhs_index == 0);  // For efficiency, the common index must be the
                               // lowest-indexed entry in the option table.
      continue;
    }
    // Add a relative constraint between the INFL values of this
    // preterminal and the LHS preterminal.
    PathTerm lhs_term(lhs_index, infl_feature_path_);
    PathTerm rhs_term(index, infl_feature_path_);
    boost::shared_ptr<RelConstraint> constraint(new RelConstraint(lhs_term,
                                                                  rhs_term));
    state_.constraint_set.rel_set().Insert(constraint);
  }
}

template<typename T, typename R, int N>
void RelationEvaluator<T,R,N>::BuildOptionTable(const R &relation) {
  state_.option_table.Clear();
  de::BitParLabel label;
  for (typename R::ConstIterator p = relation.Begin();
       p != relation.End(); ++p) {
    const T *tree = *p;
    if (!tree->IsLeaf()) {
      continue;
    }
    const typename LeafIndexMap::IdType index = state_.leaf_index_map.Lookup(tree);
    // FIXME Using boost::tuples::get because this won't compile.  Why?
    //const std::string &word = tree->label().get<N>();
    const std::string &word = boost::tuples::get<N>(tree->label());

    std::size_t word_id = vocab_.Lookup(word);
    if (word_id == Vocabulary::NullId()) {
      std::ostringstream msg;
      msg << "relation contains word `" << word << "' that is not in lexicon";
      throw Exception(msg.str());
    }

    // Create a feature structure specifying the POS type of word in order
    // to restrict the lexicon lookup.
    // TODO Since there's only a small set of POS tags, it would make
    // sense to cache the feature structures.  An unordered_map from
    // std::string (label.cat) to boost::shared_ptr<FeatureStructure> should
    // work.
    label_parser_.Parse(boost::tuples::get<N>(tree->parent()->label()), label);
    AtomicValue pos_atom = value_set_.Lookup(label.cat);
    FeatureStructureSpec spec;
    spec.content_pairs.insert(std::make_pair(cat_feature_path_, pos_atom));
    boost::shared_ptr<FeatureStructure> fs(new FeatureStructure(spec));

    state_.fs_vec.clear();
    if (!lexicon_.Lookup(word_id, *fs, std::back_inserter(state_.fs_vec))) {
      // TODO Allow user to provide a callback to handle this?
      std::ostringstream msg;
      msg << "lexicon is missing entry for `" << word
          << "' with POS value `" << label.cat << "'";
      throw Exception(msg.str());
    }
    state_.option_table.AddColumn(index, state_.fs_vec.begin(),
                                  state_.fs_vec.end());
  }
}

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
