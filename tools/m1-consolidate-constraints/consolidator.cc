#include "consolidator.h"

#include "options.h"

#include "taco/text-formats/constraint_set_seq_parser.h"
#include "taco/text-formats/feature_structure_seq_parser.h"

#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <iostream>

namespace taco {
namespace tool {
namespace m1 {

Consolidator::Consolidator(
    const Options &options,
    Vocabulary &symbol_set,
    Vocabulary &feature_set,
    Vocabulary &value_set,
    ConstraintTableWriter &table_writer,
    ConstraintMapWriter &map_writer)
    : options_(options)
    , next_id_(1)
    , batch_(feature_set)
    , symbol_set_(symbol_set)
    , feature_set_(feature_set)
    , value_set_(value_set)
    , table_writer_(table_writer)
    , map_writer_(map_writer)
    , batch_start_(-1)
    , batch_end_(-1)
    , num_conflict_reports_(0) {
}

void Consolidator::ProcessEntry(const ConstraintExtractParser::Entry &entry,
                                int line_num) {
  // Determine if this entry begins a new batch.
  if (batch_.IsEmpty()) {
    batch_.set_lhs(entry.lhs);
    batch_.set_rhs(entry.rhs);
    batch_start_ = line_num;
  } else if (entry.lhs != batch_.lhs() ||
             entry.rhs.size() != batch_.rhs().size() ||
             !std::equal(entry.rhs.begin(), entry.rhs.end(),
                         batch_.rhs().begin())) {
    batch_end_ = line_num - 1;
    ProcessBatch();
    batch_.Clear();
    batch_.set_lhs(entry.lhs);
    batch_.set_rhs(entry.rhs);
    batch_start_ = line_num;
    batch_end_ = -1;
  }

  // Parse the sequence of constraint sets.
  cs_seq_.clear();
  {
    ConstraintSetSeqParser end;
    ConstraintSetSeqParser p(entry.constraint_sets, feature_set_, value_set_); 
    while (p != end) {
      cs_seq_.push_back(*p++);
    }
  }

  // Add the sequence pair to the batch.
  batch_.Add(cs_seq_);
}

void Consolidator::RepeatPrevious() {
  batch_.RepeatPrevious();
}

void Consolidator::Finish(int line_num) {
  batch_end_ = line_num;
  ProcessBatch();
  batch_.Clear();
  batch_start_ = -1;
  batch_end_ = -1;
}

void Consolidator::ReportConflict() {
  // FIXME Use tool's Warn()
  std::cerr << "Conflict: start = " << batch_start_
            << ", end = " << batch_end_ << std::endl;
  ++num_conflict_reports_;
}

void Consolidator::ProcessBatch() {
  ++stats_.num_target_sides;
  ++stats_.target_side_counts[batch_.Size()];
  if (!batch_.HasCandidates()) {
    return;
  }
  stats_.num_pre_merge_winners += batch_.CountWinners(options_.majority);
  stats_.weighted_pre_merge_winners += (batch_.CountWinners(options_.majority) * batch_.Size());
  batch_.Consolidate(cs_seq_, options_.majority, options_.minority);
  stats_.num_post_merge_winners += cs_seq_.size();
  stats_.weighted_post_merge_winners += (cs_seq_.size() * batch_.Size());
  if (cs_seq_.empty()) {
    if (options_.max_conflict_reports == -1 ||
        num_conflict_reports_ < options_.max_conflict_reports) {
      ReportConflict();
    }
    return;
  }
  std::vector<unsigned int> ids;
  CSVec::const_iterator p = cs_seq_.begin();
  CSVec::const_iterator p_end = cs_seq_.end();
  while (p != p_end) {
    unsigned int &id = id_map_[*p];
    if (!id) {
      id = next_id_++;
      table_writer_.Write(id, **p);
    }
    ids.push_back(id);
    ++p;
  }
  CreateKey(batch_.lhs(), batch_.rhs(), symbol_set_, tmp_key_);
  map_writer_.Write(tmp_key_, ids);
}

void Consolidator::CreateKey(const std::string &lhs,
                             const std::vector<std::string> &rhs,
                             const Vocabulary &symbol_set,
                             std::string &key) {
  key = boost::lexical_cast<std::string>(symbol_set.Lookup(lhs));
  for (std::vector<std::string>::const_iterator p = rhs.begin();
       p != rhs.end(); ++p) {
    key += "-";
    key += boost::lexical_cast<std::string>(symbol_set.Lookup(*p));
  }
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
