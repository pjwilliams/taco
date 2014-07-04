#ifndef TACO_TOOLS_M1_ESTIMATE_CASE_MODEL_ESTIMATE_CASE_MODEL_H_
#define TACO_TOOLS_M1_ESTIMATE_CASE_MODEL_ESTIMATE_CASE_MODEL_H_

#include "typedef.h"

#include "tools-common/compat-nlp-de/bitpar.h"
#include "tools-common/cli/tool.h"
#include "tools-common/m1/parse_tree_type.h"

namespace taco {
namespace tool {
namespace m1 {

struct Options;

class EstimateCaseFreqs : public Tool {
 public:
  EstimateCaseFreqs() : Tool("m1-estimate-case-freqs") {}
  virtual int Main(int, char *[]);
 private:
  void ProcessOptions(int, char *[], Options &) const;
  void FindNounPhrase(const Relation &, ParseTreeType, std::string &) const;

  de::BitParLabelParser label_parser_;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
