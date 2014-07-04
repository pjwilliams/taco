#include "batch.h"

namespace taco {
namespace tool {
namespace m1 {

Batch::Batch(Vocabulary &feature_set)
    : rule_count_(0)
    , cs_merger_(feature_set) {
}

void Batch::set_lhs(const StringPiece &s) {
  lhs_ = s.as_string();
}

void Batch::set_rhs(const std::vector<StringPiece> &v) {
  rhs_.clear();
  for (std::size_t i = 0; i < v.size(); ++i) {
    rhs_.push_back(v[i].as_string());
  }
}

int Batch::Size() const {
  return rule_count_;
}

bool Batch::IsEmpty() const {
  return rule_count_ == 0;
}

bool Batch::HasCandidates() const {
  return !candidates_.empty();
}

void Batch::Clear() {
  lhs_.clear();
  rhs_.clear();
  candidates_.clear();
  rule_count_ = 0;
}

void Batch::Add(CSVec &cs_vec) {
  ++rule_count_;
  prev_counts_.clear();

  const CSVec::iterator p_end = cs_vec.end();
  for (CSVec::iterator p = cs_vec.begin(); p != p_end; ++p) {
    boost::shared_ptr<const ConstraintSet> cs = *p;
    indices_.clear();
    cs->GetIndices(indices_);
    // Insert CS and increment its count.
    int &count = candidates_[indices_][cs];
    ++count;
    // Record the address of the count in case RepeatPrevious() is called.
    prev_counts_.push_back(&count);
  }
}

void Batch::RepeatPrevious() {
  ++rule_count_;
  for (std::vector<int *>::iterator p = prev_counts_.begin();
       p != prev_counts_.end(); ++p) {
    int &count = **p;
    ++count;
  }
}

int Batch::CountWinners(float required_majority) const {
  int count = 0;
  for (Map::const_iterator p = candidates_.begin();
       p != candidates_.end(); ++p) {
    const InnerMap &inner = p->second;
    for (InnerMap::const_iterator q = inner.begin(); q != inner.end(); ++q) {
      float ratio = static_cast<float>(q->second) / rule_count_;
      if (ratio >= required_majority) {
        ++count;
      }
    }
  }
  return count;
}

void Batch::Consolidate(CSVec &cs_vec,
                        float required_majority,
                        float required_minority) const {
  cs_vec.clear();
  for (Map::const_iterator p = candidates_.begin();
       p != candidates_.end(); ++p) {
    const InnerMap &inner = p->second;
    // Calculate the total CS count for this index set.
    int total_for_index_set = 0;
    for (InnerMap::const_iterator q = inner.begin(); q != inner.end(); ++q) {
      total_for_index_set += q->second;
    }
    float ratio = static_cast<float>(total_for_index_set) / rule_count_;
    // If the count is not high enough to possibly get a majority then
    // skip this index set.
    if (ratio < required_majority) {
      continue;
    }
    // Add the first (non-rare) constraint set to counted_vec_.
    counted_vec_.clear();
    InnerMap::const_iterator q;
    for (q = inner.begin(); q != inner.end(); q++) {
      ratio = static_cast<float>(q->second) / rule_count_;
      if (ratio >= required_minority) {
        counted_vec_.push_back(std::make_pair(q->first, q->second));
        ++q;
        break;
      }
    }
    // Attempt to merge the remaining constraint sets (greedily).
    for (; q != inner.end(); ++q) {
      // Ignore rare constraint sets.
      ratio = static_cast<float>(q->second) / rule_count_;
      if (ratio < required_minority) {
        continue;
      }
      // Try to merge each CS with those already in counted_vec_ in turn.
      std::size_t i = 0;
      for (; i < counted_vec_.size(); ++i) {
        boost::shared_ptr<ConstraintSet> merged_cs;
        if (cs_merger_.Merge(*(counted_vec_[i].first), *(q->first),
                             merged_cs)) {
          counted_vec_[i].first = merged_cs;
          counted_vec_[i].second += q->second;
          break;
        }
      }
      // If the new CS couldn't be merged with any existing entry then add it
      // as is.
      if (i == counted_vec_.size()) {
        counted_vec_.push_back(std::make_pair(q->first, q->second));
      }
    }
    // Look for a winner.
    for (std::size_t i = 0; i < counted_vec_.size(); ++i) {
      ratio = static_cast<float>(counted_vec_[i].second) / rule_count_;
      if (ratio >= required_majority) {
        // There can only be one winner.
        // FIXME At least that would be true if required_majority was
        // required to be > 0.5.  Need to fix that.
        cs_vec.push_back(counted_vec_[i].first);
        break;
      }
    }
    counted_vec_.clear();
  }
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
