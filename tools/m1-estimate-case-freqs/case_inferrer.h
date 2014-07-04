#ifndef TACO_TOOLS_M1_ESTIMATE_CASE_MODEL_CASE_INFERRER_H_
#define TACO_TOOLS_M1_ESTIMATE_CASE_MODEL_CASE_INFERRER_H_

#include "typedef.h"

#include "tools-common/compat-nlp-de/bitpar.h"
#include "tools-common/m1/relation_evaluator.h"

#include "taco/constraint_evaluator.h"
#include "taco/feature_structure.h"
#include "taco/interpretation.h"
#include "taco/lexicon.h"
#include "taco/base/vocabulary.h"

#include <set>

namespace taco {
namespace tool {
namespace m1 {

class CaseInferrer {
 public:
  CaseInferrer(const Lexicon<std::size_t> &, const Vocabulary &,
               const Vocabulary &, Feature, Feature, Feature);

  void Infer(const Relation &, std::set<AtomicValue> &);

 private:
  RelationEvaluatorT relation_evaluator_;
  FeaturePath case_feature_path_;
  std::vector<Interpretation> interpretations_;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
