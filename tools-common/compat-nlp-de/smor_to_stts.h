#ifndef TACO_TOOLS_COMMON_COMPAT_NLP_DE_SMOR_TO_STTS_H_
#define TACO_TOOLS_COMMON_COMPAT_NLP_DE_SMOR_TO_STTS_H_

#include <set>

#include "tools-common/compat-nlp-de/smor.h"
#include "tools-common/compat-nlp-de/stts.h"

namespace taco {
namespace tool {
namespace de {

class SmorToSTTS {
public:
  void DerivePosTags(const smor::Analysis &, std::set<stts::Tag> &) const;
};

}  // namespace de
}  // namespace tool
}  // namespace taco

#endif
