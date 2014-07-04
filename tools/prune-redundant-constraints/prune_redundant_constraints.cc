#include "prune_redundant_constraints.h"

#include "options.h"

#include "tools-common/compat-moses/rule_table_parser.h"

#include "taco/base/string_piece.h"
#include "taco/text-formats/constraint_set_parser.h"
#include "taco/text-formats/constraint_table_parser.h"

#include <boost/program_options.hpp>

#include <iostream>
#include <sstream>

namespace taco {
namespace tool {

int PruneRedundantConstraints::Main(int argc, char *argv[]) {
  using moses::RuleTableParser;

  // Process command-line options.
  Options options;
  ProcessOptions(argc, argv, options);

  // Get the rule table input stream.
  std::istream &input_stream = OpenInputOrDie("-");

  // Open the constraint table stream.
  std::ifstream constraint_table_stream;
  OpenNamedInputOrDie(options.constraint_table_file, constraint_table_stream);

  // Open the feature selection table stream.
  std::ifstream feature_selection_table_stream;
  OpenNamedInputOrDie(options.feature_selection_table_file,
                      feature_selection_table_stream);
  
  // Open the output stream.
  std::ostream &output = OpenOutputOrDie(options.output_file);

  // Load the tables into memory.
  LoadConstraintTable(constraint_table_stream);
  LoadFeatureSelectionTable(feature_selection_table_stream);

  typedef std::pair<int, int> IdPair;
  std::vector<IdPair> retained_ids;
  std::set<int> indices;

  RuleTableParser end;
  const int fields = RuleTableParser::ALL_CM;
  for (RuleTableParser parser(input_stream, fields); parser != end; ++parser) {
    const RuleTableParser::Entry &entry = *parser;

    int feature_selection_id =
        std::atoi(entry.feature_selection_id.as_string().c_str());

    const FeatureSelectionRule &selection_rule =
        *(feature_selection_table_[feature_selection_id]);

    // Assumption: no CS other than the root can be pruned.
    // If the root feature value is not dropped then we can't prune anything.
    if (selection_rule.type != FeatureSelectionRule::Rule_Drop) {
      output << entry.line << std::endl;
      continue;
    }

    retained_ids.clear();
    for (std::vector<std::pair<StringPiece,StringPiece> >::const_iterator p =
           entry.constraint_ids.begin(); p != entry.constraint_ids.end(); ++p) {
      IdPair id_pair;
      id_pair.first = std::atoi(p->first.as_string().c_str());
      id_pair.second = std::atoi(p->second.as_string().c_str());
      if (id_pair.first != 0) {  // FIXME Should support multiple constraint talbes
        retained_ids.push_back(id_pair);
        continue;
      }
      const ConstraintSet &cs = *(constraint_table_[id_pair.second]);
      // Assumption: failure is possible for all non-root CSs.
      if (!cs.ContainsRoot()) {
        retained_ids.push_back(id_pair);
        continue;
      }
      indices.clear();
      cs.GetIndices(indices);
      if (!IsFullyLexical(entry.target_rhs, indices)) {
        retained_ids.push_back(id_pair);
      }
    }

    if (retained_ids.size() == entry.constraint_ids.size()) {
      // If no IDs were pruned then write the rule as-is.
      output << entry.line << std::endl;
    } else {
      // Otherwise update the rule.
      WriteAdjustedRule(entry.line, retained_ids, feature_selection_id, output);
    }
  }

  return 0;
}

void PruneRedundantConstraints::WriteAdjustedRule(
    const std::string &line,
    const std::vector<std::pair<int,int> > &new_constraint_ids,
    int feature_selection_id,
    std::ostream &output) {
  std::size_t pos = line.rfind("|||");
  if (pos == std::string::npos) {
    throw Exception("no delimiters found");
  }
  if (pos == 0) {
    throw Exception("first field is empty");
  }
  pos = line.rfind("|||", pos-1);
  if (pos == std::string::npos) {
    throw Exception("only one delimiter found");
  }
  output << line.substr(0, pos) << "|||";
  for (std::vector<std::pair<int, int> >::const_iterator p = new_constraint_ids.begin();
       p != new_constraint_ids.end(); ++p) {
    output << " " << p->first << ":" << p->second;
  }
  output << " ||| " << feature_selection_id << std::endl;
}

void PruneRedundantConstraints::LoadConstraintTable(std::istream &input) {
  assert(constraint_table_.empty());

  // The constraint ID origin is 1, so insert an empty constraint set pointer
  // at index 0.
  // FIXME Sort this origin business out.  It's a constant source of errors.
  constraint_table_.resize(1);

  ConstraintSetParser cs_parser(feature_set_, value_set_);

  ConstraintTableParser end;
  for (ConstraintTableParser parser(input); parser != end; ++parser) {
    const ConstraintTableParser::Entry &entry = *parser;
    // Check that the constraint ID is the same as the next table index.
    size_t id = std::atoi(entry.id.as_string().c_str());
    assert(id == constraint_table_.size());
    boost::shared_ptr<ConstraintSet> cs = cs_parser.Parse(entry.constraint_set);
    constraint_table_.push_back(cs);
  }
}

void PruneRedundantConstraints::LoadFeatureSelectionTable(std::istream &input) {
  assert(feature_selection_table_.empty());

  FeatureSelectionTableParser end;
  for (FeatureSelectionTableParser parser(input, feature_set_);
       parser != end; ++parser) {
    const FeatureSelectionTableParser::Entry &entry = *parser;
    // Check that the feature selection ID is the same as the next table index.
    assert(entry.index == static_cast<int>(feature_selection_table_.size()));
    feature_selection_table_.push_back(entry.rule);
  }
}

bool PruneRedundantConstraints::IsFullyLexical(
    const std::vector<StringPiece> &target_rhs,
    const std::set<int> &indices) const {
  for (std::set<int>::const_iterator p = indices.begin();
       p != indices.end(); ++p) {
    if (*p == 0) {
      continue;
    }
    if (IsNonTerminal(target_rhs[*p-1])) {
      return false;
    }
  }
  return true;
}

bool PruneRedundantConstraints::IsNonTerminal(const StringPiece &s) const {
  const std::size_t len = s.size();
  return len >= 2 && s[0] == '[' && s[len-1] == ']';
}

void PruneRedundantConstraints::ProcessOptions(int argc, char *argv[],
                                               Options &options) const {
  namespace po = boost::program_options;

  std::ostringstream usage_top;
  usage_top << "Usage: " << name()
            << " [OPTION]... CONSTRAINT-TABLE FEATURE-SELECTION-TABLE\n\n"
            << "TODO\n\n"
            << "Options";

  // Construct the 'bottom' of the usage message.
  std::ostringstream usage_bottom;
  usage_bottom << "";
  // TODO Add some examples

  // Declare the command line options that are visible to the user.
  po::options_description visible(usage_top.str());
  visible.add_options()
    ("help",
        "print help message and exit")
    ("output,o",
        po::value(&options.output_file),
        "write to arg instead of standard output")
  ;

  // Declare the command line options that are hidden from the user
  // (these are used as positional options).
  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("constraint-table-file",
        po::value(&options.constraint_table_file),
        "constraint table file")
    ("feature-selection-table-file",
        po::value(&options.feature_selection_table_file),
        "feature selection table file")
  ;

  // Compose the full set of command-line options.
  po::options_description cmd_line_options;
  cmd_line_options.add(visible).add(hidden);

  // Register the positional options.
  po::positional_options_description p;
  p.add("constraint-table-file", 1);
  p.add("feature-selection-table-file", 1);

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
  if (!vm.count("constraint-table-file") ||
      !vm.count("feature-selection-table-file")) {
    std::ostringstream msg;
    msg << "missing required argument\n\n" << visible << usage_bottom.str()
        << std::endl;
    Error(msg.str());
  }
}

}  // namespace tool
}  // namespace taco
