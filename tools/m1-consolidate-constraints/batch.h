#ifndef TACO_TOOLS_M1_CONSOLIDATE_CONSTRAINTS_BATCH_H_
#define TACO_TOOLS_M1_CONSOLIDATE_CONSTRAINTS_BATCH_H_

#include "cs_merger.h"
#include "typedef.h"

#include "taco/constraint_set_set.h"
#include "taco/base/string_piece.h"
#include "taco/base/utility.h"
#include "taco/base/vocabulary.h"

#include <boost/shared_ptr.hpp>

#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace taco {
namespace tool {
namespace m1 {

class Batch {
 public:
  Batch(Vocabulary &);

  const std::string &lhs() { return lhs_; }
  const std::vector<std::string> &rhs() { return rhs_; }

  void set_lhs(const StringPiece &);
  void set_rhs(const std::vector<StringPiece> &);

  int Size() const;

  bool IsEmpty() const;

  bool HasCandidates() const;

  void Clear();

  void Add(CSVec &);

  void RepeatPrevious();

  int CountWinners(float) const;

  void Consolidate(CSVec &, float, float) const;

 private:
  typedef std::set<int> IndexSet;

  typedef DereferencingOrderer<boost::shared_ptr<const ConstraintSet>,
                               ConstraintSetOrderer> InnerMapOrderer;

  typedef std::map<boost::shared_ptr<const ConstraintSet>,
                   int,
                   InnerMapOrderer> InnerMap;

  typedef std::map<IndexSet, InnerMap> Map;

  std::string lhs_;
  std::vector<std::string> rhs_;
  Map candidates_;
  int rule_count_;
  IndexSet indices_;
  std::vector<int *> prev_counts_;
  mutable CountedCSVec counted_vec_;
  mutable CSMerger cs_merger_;
};

}  // namespace taco
}  // namespace tool
}  // namespace taco

#endif
