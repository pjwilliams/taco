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
  Extractor(const Options &options) : options_(options) {}
  ~Extractor() {}

  void Extract(Tree &, std::set<Relation> &, std::vector<std::string> &);

 private:
  typedef boost::shared_ptr<Relation> RelationSP;
  typedef boost::unordered_map<const Tree *, Relation *> NodeToRelationMap;
  typedef std::vector<RelationSP> RelationVec;

  void ApplyNounModifierRule(Tree &, std::vector<std::string> &);
  void ApplySubjectVerbRule(Tree &, std::vector<std::string> &);

  Tree *ClosestLeftSibling(const Tree &);
  Tree *ClosestRightSibling(const Tree &);

  const Tree *Dominates(const Tree &, const std::string &);

  void PnReRule(Tree &, std::vector<std::string> &);
  void PnDmRuleB(Tree &, std::vector<std::string> &);

  bool IsAj(const Tree &);
  bool IsCat(const Tree &, const std::string &);
  bool IsDet(const Tree &);
  bool IsModifier(const Tree &);
  bool IsNoun(const Tree &);
  bool IsPos(const Tree &, const std::string &);
  bool IsSubject(const Tree &);

  void AddSubtreeToRelation(Tree &, const std::string &, Relation &);
  void AddNodeToRelation(Tree &, Relation &);

  NodeToRelationMap node_to_relation_;
  RelationVec relation_vec_;
  const Options &options_;
};

}  // namespace m3
}  // namespace tool
}  // namespace taco

#endif
