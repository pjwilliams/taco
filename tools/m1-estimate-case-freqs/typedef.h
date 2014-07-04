#ifndef TACO_TOOLS_M1_ESTIMATE_CASE_MODEL_TYPEDEF_H_
#define TACO_TOOLS_M1_ESTIMATE_CASE_MODEL_TYPEDEF_H_

#include "tools-common/m1/relation_evaluator.h"
#include "tools-common/relation/relation.h"
#include "tools-common/relation/relation_id.h"

#include "tools-common/syntax-tree/syntax_tree.h"

#include <boost/tuple/tuple.hpp>

#include <string>

namespace taco {
namespace tool {
namespace m1 {

const int kIdxCat = 0;
const int kIdxId = 1;

typedef boost::tuple<std::string, RelationId> Label;
typedef SyntaxTree<Label> Tree;

typedef Relation<const Tree *> Relation;

typedef RelationEvaluator<Tree, Relation, kIdxCat> RelationEvaluatorT;

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
