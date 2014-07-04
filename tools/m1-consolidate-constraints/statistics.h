#ifndef TACO_TOOLS_M1_CONSOLIDATE_CONSTRAINTS_STATISTICS_H_
#define TACO_TOOLS_M1_CONSOLIDATE_CONSTRAINTS_STATISTICS_H_

#include <boost/unordered_map.hpp>

namespace taco {
namespace tool {
namespace m1 {

struct Statistics {
  typedef boost::unordered_map<int, int> CountMap;

  Statistics()
      : num_target_sides(0)
      , num_pre_merge_winners(0)
      , num_post_merge_winners(0)
      , weighted_pre_merge_winners(0)
      , weighted_post_merge_winners(0) {}

  int num_target_sides;
  int num_pre_merge_winners;
  int num_post_merge_winners;
  int weighted_pre_merge_winners;
  int weighted_post_merge_winners;
  CountMap target_side_counts;
};

}  // namespace taco
}  // namespace tool
}  // namespace taco

#endif
