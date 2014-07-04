#ifndef TACO_SRC_TACO_FEATURE_TREE_H_
#define TACO_SRC_TACO_FEATURE_TREE_H_

#include <map>

#include <boost/shared_ptr.hpp>

#include "taco/base/basic_types.h"

namespace taco {

// Represents a prefix tree of feature paths.
struct FeatureTree {
 public:
  typedef std::map<Feature, boost::shared_ptr<FeatureTree> > ChildMap;
  bool Empty() const { return children_.empty(); }
  ChildMap children_;
};

}  // namespace taco

#endif
