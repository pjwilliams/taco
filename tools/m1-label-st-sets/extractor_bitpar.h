#ifndef TACO_TOOLS_M1_LABEL_ST_SETS_EXTRACTOR_BITPAR_H_
#define TACO_TOOLS_M1_LABEL_ST_SETS_EXTRACTOR_BITPAR_H_

#include "extractor.h"
#include "options.h"
#include "typedef.h"

#include "tools-common/compat-nlp-de/bitpar.h"

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include <string>
#include <vector>

namespace taco {
namespace tool {
namespace m1 {

class ExtractorBitPar : public Extractor {
 public:
  ExtractorBitPar(const Options &options) : Extractor(options) {}

  void Extract(Tree &, std::set<Relation> &, std::vector<std::string> &);

 private:
  typedef boost::shared_ptr<Relation> RelationSP;
  typedef boost::unordered_map<const Tree *, Relation *> NodeToRelationMap;
  typedef std::vector<RelationSP> RelationVec;

  void ExtractNpPp(Tree &, std::vector<std::string> &);
  void ExtractSubjVerb(Tree &, std::vector<std::string> &);

  bool IsComma(const Tree &);
  bool IsFiniteVerb(const Tree &);
  bool IsModifier(const Tree &);
  bool IsNounOrPronoun(const Tree &);
  bool IsPreposition(const Tree &);
  bool IsProperNoun(const Tree &);
  bool IsSubject(const Tree &);

  Tree *FindNounPhraseHead(Tree &);

  void AddNodeToRelation(Tree &, Relation &);
  void AddWordToRelation(Tree &, Relation &);
  void AddNpPpToRelation(Tree &, Relation &);

  void AddClauseToRelation(Tree &);

  de::BitParLabel label_;
  de::BitParLabelParser label_parser_;
  NodeToRelationMap node_to_relation_;
  RelationVec relation_vec_;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
