#include "m1_consolidate_constraints.h"

#include "consolidator.h"
#include "options.h"
#include "statistics.h"

#include "tools-common/text-formats/constraint_extract_parser.h"
#include "tools-common/text-formats/constraint_map_writer.h"
#include "tools-common/text-formats/vocab_parser.h"

#include "taco/constraint_set.h"
#include "taco/text-formats/constraint_table_writer.h"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

namespace taco {
namespace tool {
namespace m1 {

int ConsolidateConstraints::Main(int argc, char *argv[]) {
  // Process command-line options.
  Options options;
  ProcessOptions(argc, argv, options);

  // Open the input streams.
  std::ifstream extract_stream;
  std::ifstream vocab_stream;
  OpenNamedInputOrDie(options.extract_file, extract_stream);
  OpenNamedInputOrDie(options.vocab_file, vocab_stream);

  // Open the output streams.
  std::ofstream table_stream;
  std::ofstream map_stream;
  OpenNamedOutputOrDie(options.table_file, table_stream);
  OpenNamedOutputOrDie(options.map_file, map_stream);

  // Load the symbol set from the vocab file.
  Vocabulary symbol_set;
  VocabParser vocab_end;
  for (VocabParser p(vocab_stream); p != vocab_end; ++p) {
    const StringPiece &symbol = p->symbol;
    symbol_set.Insert(symbol.as_string());
  }

  // Create the various output writers.
  Vocabulary feature_set;
  Vocabulary value_set;
  ConstraintWriter constraint_writer(feature_set, value_set);
  ConstraintTableWriter table_writer(constraint_writer, table_stream);
  ConstraintMapWriter map_writer(map_stream);

  // TODO Are objects in the same scope guaranteed to be destroyed in reverse
  // order of creation?  The constraint consolidator requires the various
  // writers to still be around when it is destructed.
  Consolidator consolidator(options, symbol_set, feature_set, value_set,
                            table_writer, map_writer);

  ConstraintExtractParser end;
  std::size_t line_num = 1;
  try {
    for (ConstraintExtractParser parser(extract_stream);
         parser != end; ++parser) {
      const ConstraintExtractParser::Entry &entry = *parser;
      if (entry.is_identical_to_previous) {
        consolidator.RepeatPrevious();
      } else {
        consolidator.ProcessEntry(entry, line_num);
      }
      if (line_num % 1000000 == 0) {
        PrintStats(consolidator.stats(), line_num);
      }
      ++line_num;
    }
    consolidator.Finish(line_num-1);
    PrintStats(consolidator.stats(), line_num-1);
    PrintCounts(consolidator.stats());
  } catch (const Exception &e) {
    std::ostringstream msg;
    msg << "line " << line_num << ": " << e.msg();
    Error(msg.str());
  }

  return 0;
}

void ConsolidateConstraints::PrintStats(const Statistics &stats,
                                        int line_num) {
  std::ostringstream msg;
  msg << "Stats at line " << line_num << ": "
      << stats.num_target_sides << " / "
      << stats.num_pre_merge_winners << " / "
      << stats.num_post_merge_winners << " / "
      << stats.weighted_pre_merge_winners << " / "
      << stats.weighted_post_merge_winners;
  Warn(msg.str());
}

void ConsolidateConstraints::PrintCounts(const Statistics &stats) {
  const Statistics::CountMap &map = stats.target_side_counts;
  for (Statistics::CountMap::const_iterator p = map.begin();
       p != map.end(); ++p) {
    std::cerr << p->first << " " << p->second << std::endl;
  }
}

void ConsolidateConstraints::ProcessOptions(int argc, char *argv[],
                                            Options &options) const {
  namespace po = boost::program_options;

  std::ostringstream usage_top;
  usage_top << "Usage: " << name() << " [OPTIONS] EXTRACT VOCAB TABLE MAP\n\n"
            << "Blah\n\n"
            << "Options";

  // Construct the 'bottom' of the usage message.
  std::ostringstream usage_bottom;
  usage_bottom << "";
  // TODO Add some examples

  // Declare the command line options that are visible to the user.
  po::options_description visible(usage_top.str());
  visible.add_options()
    ("help",
        "print this help message and exit")
    // TODO Report default value
    ("majority",
        po::value(&options.majority),
        "set the required majority for conflict resolution")
    ("max-conflict-reports",
        po::value(&options.max_conflict_reports),
        "maximum number of conflicts to report")
  ;

  // Declare the command line options that are hidden from the user
  // (these are used as positional options).
  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("extract-file",
        po::value(&options.extract_file),
        "constraint extract file")
    ("vocab-file",
        po::value(&options.vocab_file),
        "vocab file")
    ("table-file",
        po::value(&options.table_file),
        "constraint table file")
    ("map-file",
        po::value(&options.map_file),
        "constraint map file")
  ;

  // Compose the full set of command-line options.
  po::options_description cmd_line_options;
  cmd_line_options.add(visible).add(hidden);

  // Register the positional options.
  po::positional_options_description p;
  p.add("extract-file", 1);
  p.add("vocab-file", 1);
  p.add("table-file", 1);
  p.add("map-file", 1);

  // Process the command-line.
  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).style(CommonOptionStyle()).
              options(cmd_line_options).positional(p).run(), vm);
    po::notify(vm);
  } catch (const std::exception &e) {
    std::ostringstream msg;
    msg << e.what() << "\n\n" << visible << usage_bottom.str();
    Error(msg.str());
  }

  if (vm.count("help")) {
    std::cout << visible << usage_bottom.str() << std::endl;
    std::exit(0);
  }

  // Check positional options were given.
  if (!vm.count("extract-file") || !vm.count("vocab-file") ||
      !vm.count("table-file") || !vm.count("map-file")) {
    std::ostringstream msg;
    msg << "missing required argument\n\n" << visible << usage_bottom.str()
        << std::endl;
    Error(msg.str());
  }
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
