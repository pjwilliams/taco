#ifndef TACO_SRC_TACO_TEXT_FORMATS_FEATURE_STRUCTURE_WRITER_H_
#define TACO_SRC_TACO_TEXT_FORMATS_FEATURE_STRUCTURE_WRITER_H_

#include <ostream>

#include "taco/base/vocabulary.h"
#include "taco/feature_structure.h"

namespace taco {

class FeatureStructureWriter {
 public:
  FeatureStructureWriter(const Vocabulary &feature_set,
                         const Vocabulary &value_set)
      : feature_set_(feature_set)
      , value_set_(value_set) {}

  void Write(const FeatureStructure &, std::ostream &) const;

 private:
  const Vocabulary &feature_set_;
  const Vocabulary &value_set_;
};

}  // namespace taco

#endif
