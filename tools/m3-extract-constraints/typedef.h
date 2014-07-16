#ifndef TACO_TOOLS_M3_EXTRACT_CONSTRAINTS_TYPEDEF_H_
#define TACO_TOOLS_M3_EXTRACT_CONSTRAINTS_TYPEDEF_H_

#include "tools-common/m1/case_model.h"
#include "tools-common/relation/relation.h"
#include "tools-common/relation/relation_id.h"

#include "taco/constraint_set.h"
#include "taco/feature_structure.h"
#include "tools-common/syntax-tree/syntax_tree.h"

#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#include <map>
#include <string>
#include <vector>

namespace taco {
namespace tool {
namespace m3 {

const int kIdxCat = 0;
const int kIdxId = 1;

typedef boost::tuple<std::string, RelationId> Label;
typedef SyntaxTree<Label> Tree;

typedef Relation<const Tree*> Relation;
typedef std::map<int, Relation> RelationMap;

// Map from selector-target relation ID to case model probability function.
typedef std::pair<
          const Tree*,
          const m1::CaseTable::ProbabilityFunction *
        > NounPhraseDistPair;

typedef std::map<int, NounPhraseDistPair> CaseModelMap;

typedef std::vector<boost::shared_ptr<ConstraintSet> > CSVec;

}  // namespace m3
}  // namespace tool
}  // namespace taco

#endif
