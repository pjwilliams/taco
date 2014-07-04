#ifndef TACO_SRC_TACO_FEATURE_SELECTION_RULE_H_
#define TACO_SRC_TACO_FEATURE_SELECTION_RULE_H_

#include <boost/shared_ptr.hpp>

#include "taco/feature_structure.h"

namespace taco {

struct FeatureSelectionRule {
  enum RuleType {
    Rule_Assign,
    Rule_Drop,
    Rule_Select
  };

  FeatureSelectionRule(RuleType t) : type(t) {}
  FeatureSelectionRule(RuleType t, boost::shared_ptr<FeatureTree> &tree)
      : type(t)
      , selection_tree(tree) {
  }

  RuleType type;
  boost::shared_ptr<FeatureTree> selection_tree;
};

}  // namespace taco

#endif
