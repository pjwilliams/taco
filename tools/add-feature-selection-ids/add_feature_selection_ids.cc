#include "add_feature_selection_ids.h"

#include "options.h"

#include "tools-common/compat-moses/rule_table_parser.h"
#include "tools-common/feature_selection_map.h"

#include "taco/text-formats/constraint_table_parser.h"
#include "taco/text-formats/constraint_table_writer.h"
#include "taco/base/vocabulary.h"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/unordered_map.hpp>

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

namespace taco {
namespace tool {

int AddFeatureSelectionIds::Main(int argc, char *argv[]) {
  typedef std::vector<unsigned int> Key;
  typedef std::set<unsigned int> IdSet;
  typedef boost::unordered_map<Key, IdSet> IdMap;

  // Process command-line options.
  Options options;
  ProcessOptions(argc, argv, options);

  // Open the rule table and feature map streams.
  std::ifstream rule_table_stream;
  std::ifstream feature_map_stream;
  OpenNamedInputOrDie(options.rule_table_file, rule_table_stream);
  OpenNamedInputOrDie(options.feature_map_file, feature_map_stream);

  Vocabulary vocab;
  IdMap id_map;
  Key key;
  FeatureSelectionMap feature_selection_map;

  // Parse the feature map and populate feature_selection_map.
  FeatureSelectionMapParser end2;
  for (FeatureSelectionMapParser parser(feature_map_stream);
       parser != end2; ++parser) {
    const FeatureSelectionMapParser::Entry &entry = *parser;
    feature_selection_map.insert(entry.label, entry.index);
  }

  // Parse the rule table, lookup the constraint IDs, and write the augmented
  // rule table format.

  std::pair<std::string, std::string> feature_map_key;

  moses::RuleTableParser end3;
  for (moses::RuleTableParser parser(rule_table_stream);
       parser != end3; ++parser) {
    const moses::RuleTableParser::Entry &entry = *parser;
    // Output original row from input rule table
    std::cout << entry.line;
    // Add column for feature selection rule ID
    std::cout << " |||";
    ConstructFeatureMapKey(entry.target_lhs, feature_map_key);
    int index = -1;
    if (!feature_selection_map.lookup(feature_map_key, index)) {
      std::ostringstream msg;
      msg << "no feature selection ID specified for LHS: "
          << feature_map_key.first;
      if (!feature_map_key.second.empty()) {
        msg << "-" << feature_map_key.second;
      }
      msg << " (using default ID of 0)";
      Warn(msg.str());
      index = 0;
      feature_selection_map.insert(feature_map_key, index);
    }
    std::cout << " " << index << std::endl;
  }

  return 0;
}

void AddFeatureSelectionIds::ProcessOptions(int argc, char *argv[],
                                            Options &options) const {
  namespace po = boost::program_options;

  std::ostringstream usage_top;
  usage_top << "usage: " << name() << " RULE-TABLE FEATURE-MAP\n\n"
            << "description here...\n\n"
            << "Options";

  // Construct the 'bottom' of the usage message.
  std::ostringstream usage_bottom;
  usage_bottom << "";
  // TODO Add some examples

  // Declare the command line options that are visible to the user.
  po::options_description visible(usage_top.str());
  visible.add_options()
    ("help", "print help message and exit")
    ("output,o", po::value(&options.output_file), "write to arg instead of standard output")
  ;

  // Declare the command line options that are hidden from the user
  // (these are used as positional options).
  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("rule-table-file", po::value(&options.rule_table_file), "rule table file")
    ("feature-map-file", po::value(&options.feature_map_file), "feature map file")
  ;

  // Compose the full set of command-line options.
  po::options_description cmd_line_options;
  cmd_line_options.add(visible).add(hidden);

  // Register the positional options.
  po::positional_options_description p;
  p.add("rule-table-file", 1);
  p.add("feature-map-file", 1);

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
    std::cout << visible << std::endl;
    std::exit(0);
  }

  // Check positional options were given.

  if (!vm.count("rule-table-file") || !vm.count("feature-map-file")) {
    std::ostringstream msg;
    msg << "missing required argument\n\n" << visible << usage_bottom.str() << std::endl;
    Error(msg.str());
  }
}

void AddFeatureSelectionIds::ConstructFeatureMapKey(
    const StringPiece &lhs,
    std::pair<std::string, std::string> &feature_map_key) const {
  // FIXME Rewrite this
  std::vector<std::string> parts;
  std::string stripped_lhs = lhs.substr(1, lhs.size()-2).as_string();
  boost::split(parts, stripped_lhs, boost::algorithm::is_any_of("-"));
  assert(parts.size() == 1 || parts.size() == 2);
  feature_map_key.first = parts[0];
  if (parts.size() == 2) {
    feature_map_key.second = parts[1];
  } else {
    feature_map_key.second = "";
  }
}

}  // namespace tool
}  // namespace taco
