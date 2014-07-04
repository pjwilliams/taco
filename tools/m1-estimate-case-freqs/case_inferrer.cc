#include "case_inferrer.h"

#include "taco/interpretation.h"

#include <cassert>

#include <boost/shared_ptr.hpp>

namespace taco {
namespace tool {
namespace m1 {

CaseInferrer::CaseInferrer(const Lexicon<std::size_t> &lexicon,
                           const Vocabulary &vocab,
                           const Vocabulary &value_set,
                           Feature infl_feature,
                           Feature case_feature,
                           Feature pos_feature)
    : relation_evaluator_(lexicon, vocab, value_set, infl_feature,
                          pos_feature) {
  case_feature_path_.push_back(infl_feature);
  case_feature_path_.push_back(case_feature);
}

void CaseInferrer::Infer(const Relation &relation,
                         std::set<AtomicValue> &cases) {
  assert(relation.Size() > 1);

  if (!relation_evaluator_.Evaluate(relation, interpretations_)) {
    assert(interpretations_.empty());
    return;
  }

  // TODO If there are multiple cases, maybe we should give each one a weight
  // according to its frequency in the interpretations.
  for (std::vector<Interpretation>::const_iterator p(interpretations_.begin());
       p != interpretations_.end(); ++p) {
    const Interpretation &interpretation = *p;

    // The interpretation should contain a set of identical feature
    // structures, one for each word in the relation.  Just get the
    // first one and ignore the rest.
    const std::pair<size_t, boost::shared_ptr<FeatureStructure> > &pair =
      *(interpretation.begin());
    boost::shared_ptr<FeatureStructure> fs = pair.second;
    assert(fs);

    // Look for a case value in the feature structure.
    boost::shared_ptr<const FeatureStructure> case_fs =
      fs->Get(case_feature_path_.begin(), case_feature_path_.end());
    if (case_fs) {
      cases.insert(case_fs->GetAtomicValue());
    }
  }

  interpretations_.clear();
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
