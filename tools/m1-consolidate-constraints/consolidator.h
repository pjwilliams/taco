#ifndef TACO_TOOLS_M1_CONSOLIDATE_CONSTRAINTS_CONSOLIDATOR_H_
#define TACO_TOOLS_M1_CONSOLIDATE_CONSTRAINTS_CONSOLIDATOR_H_

#include "batch.h"
#include "statistics.h"
#include "typedef.h"

#include "tools-common/text-formats/constraint_extract_parser.h"
#include "tools-common/text-formats/constraint_map_writer.h"

#include "taco/constraint_set.h"
#include "taco/feature_structure.h"
#include "taco/text-formats/constraint_table_writer.h"
#include "taco/base/utility.h"
#include "taco/base/vocabulary.h"

#include <boost/shared_ptr.hpp>

#include <vector>
#include <map>

namespace taco {
namespace tool {
namespace m1 {

struct Options;

class Consolidator {
 public:
  Consolidator(const Options &, Vocabulary &, Vocabulary &, Vocabulary &,
               ConstraintTableWriter &, ConstraintMapWriter &);

  // Process a new extract file entry.  Entries are added to a batch until the
  // key changes, at which time the batch is consolidated and a single
  // consolidated constraint set set is written out to the constraint map.
  void ProcessEntry(const ConstraintExtractParser::Entry &, int);

  // Process an extract file entry that is identical to the last one added.
  // The behaviour is the same as if ProcessEntry() were called, but this
  // should be faster.
  void RepeatPrevious();

  // Signal that the end of the input has been reached so that the final
  // batch is processed.
  void Finish(int);

  const Statistics &stats() const { return stats_; }

 private:
  void ProcessBatch();

  void CreateKey(const std::string &, const std::vector<std::string> &,
                 const Vocabulary &, std::string &);

  void ReportConflict();

  const Options &options_;
  unsigned int next_id_;
  Batch batch_;
  std::map<CS, unsigned int, CSOrderer> id_map_;
  Vocabulary &symbol_set_;
  Vocabulary &feature_set_;
  Vocabulary &value_set_;
  ConstraintTableWriter &table_writer_;
  ConstraintMapWriter &map_writer_;
  CSVec cs_seq_;
  int batch_start_;
  int batch_end_;
  int num_conflict_reports_;
  std::string tmp_key_;
  Statistics stats_;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
