#ifndef TACO_TOOLS_M1_LABEL_ST_SETS_EXTRACTOR_H_
#define TACO_TOOLS_M1_LABEL_ST_SETS_EXTRACTOR_H_

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

class Extractor {
 public:
  Extractor(const Options &options) : options_(options) {}
  virtual ~Extractor() {}

  virtual void Extract(Tree &, std::set<Relation> &,
                       std::vector<std::string> &) = 0;

 protected:
  const Options &options_;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
