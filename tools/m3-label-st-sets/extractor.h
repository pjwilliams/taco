#ifndef TACO_TOOLS_M3_LABEL_ST_SETS_EXTRACTOR_H_
#define TACO_TOOLS_M3_LABEL_ST_SETS_EXTRACTOR_H_

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "options.h"
#include "typedef.h"

namespace taco {
namespace tool {
namespace m3 {

class Extractor {
 public:
  Extractor(const Options &) {}  // Options not currently used.
  ~Extractor() {}

  void Extract(Tree &, std::set<Relation> &, std::vector<std::string> &);

 private:
  typedef boost::shared_ptr<Relation> RelationSP;
  typedef boost::unordered_map<const Tree *, Relation *> NodeToRelationMap;
  typedef std::vector<RelationSP> RelationVec;

  Tree *ClosestLeftSibling(const Tree &);

  bool DominatesAj(const Tree &);

  void Extract(Tree &, std::vector<std::string> &);

  bool IsAj(const Tree &);
  bool IsDet(const Tree &);
  bool IsModifier(const Tree &);
  bool IsNoCm(const Tree &);

  bool IsNonBranching(const Tree &);

  void AddAjSubtreeToRelation(Tree &, Relation &);
  void AddNodeToRelation(Tree &, Relation &);
  void AddUnaryChainToRelation(Tree &, Relation &);

  NodeToRelationMap node_to_relation_;
  RelationVec relation_vec_;
};

}  // namespace m3
}  // namespace tool
}  // namespace taco

#endif
