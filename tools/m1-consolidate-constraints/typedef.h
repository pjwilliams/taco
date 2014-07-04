#ifndef TACO_TOOLS_M1_CONSOLIDATE_CONSTRAINTS_TYPEDEF_H_
#define TACO_TOOLS_M1_CONSOLIDATE_CONSTRAINTS_TYPEDEF_H_

#include "taco/constraint_set.h"

#include <boost/shared_ptr.hpp>

#include <vector>

namespace taco {
namespace tool {
namespace m1 {

typedef boost::shared_ptr<const ConstraintSet> CS;
typedef DereferencingOrderer<CS, ConstraintSetOrderer> CSOrderer;
typedef std::vector<CS> CSVec;
typedef std::vector<std::pair<CS, int> > CountedCSVec;

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
