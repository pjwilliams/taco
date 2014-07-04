#ifndef TACO_TOOLS_M1_LABEL_ST_SETS_TYPEDEF_H_
#define TACO_TOOLS_M1_LABEL_ST_SETS_TYPEDEF_H_

#include "tools-common/relation/relation.h"
#include "tools-common/relation/relation_id.h"

#include "tools-common/syntax-tree/syntax_tree.h"

#include <boost/tuple/tuple.hpp>

#include <map>
#include <string>

namespace taco {
namespace tool {
namespace m1 {

const int kIdxCat = 0;
const int kIdxId = 1;
const int kIdxUnused = 2;

typedef std::map<std::string, std::string> AttrMap;
typedef boost::tuple<std::string, RelationId, AttrMap> Label;
typedef SyntaxTree<Label> Tree;

typedef Relation<Tree *> Relation;
typedef std::set<Relation> RelationSet;
typedef std::map<int, const Relation *> RelationMap;

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
