#ifndef TACO_TOOLS_M3_EXTRACT_CONSTRAINTS_EXTRACTOR_H_
#define TACO_TOOLS_M3_EXTRACT_CONSTRAINTS_EXTRACTOR_H_

#include "tree_context.h"
#include "tree_fragment.h"
#include "typedef.h"

#include "taco/constraint_set_set.h"
#include "taco/feature_structure.h"
#include "taco/base/vocabulary.h"

#include <vector>

namespace taco {
namespace tool {
namespace m3 {

struct Options;

// Extracts a set of constraint sets given the tree fragment corresponding
// to the target-side of an SCFG rule.
class Extractor {
 public:
  Extractor(Vocabulary &, Vocabulary &, const Options &);

  void Extract(const TreeFragment &, const TreeContext &, CSVec &);

 protected:
  typedef std::map<const Tree *, int> ConstraintIndexMap;

  void BuildConstraintIndexMap(const TreeFragment &,
                               ConstraintIndexMap &) const;

  Vocabulary &feature_set_;
  Vocabulary &value_set_;
  const Options &options_;
  ConstraintIndexMap constraint_index_map_;
  FeaturePath cat_path_;
  FeaturePath agr_path_;
  FeaturePath case_path_;
};

}  // namespace m3
}  // namespace tool
}  // namespace taco

#endif
