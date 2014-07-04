#include "match_constraints_to_rules.h"

#include "options.h"

#include "tools-common/text-formats/constraint_map_parser.h"
#include "tools-common/text-formats/rule_table_index_parser.h"

#include "taco/base/exception.h"
#include "taco/base/string_piece.h"

#include <boost/program_options.hpp>

#include <iostream>
#include <sstream>

namespace taco {
namespace tool {

// Input reader for the two input types.  Checks order consistency to ensure
// that input was sorted according to the criteria that the matching algorithm
// assumes.
template<typename Parser>
struct MatchConstraintsToRules::InputReader {
  InputReader(Parser f, Parser l, const std::string &d)
      : first(f)
      , last(l)
      , input_desc(d)
      , line_num(1) {
    if (first != last) {
      key = first->key.as_string();
    }
  }

  operator bool() const { return first != last; }

  bool ReadLine() {
    if (++first == last) {
      return false;
    }
    ++line_num;
    std::string new_key(first->key.as_string());
    if (new_key.compare(key) < 0) {
      std::ostringstream msg;
      msg << input_desc << " is not correctly ordered between lines "
          << line_num-1 << " and " << line_num << std::endl;
      throw Exception(msg.str());
    }
    key = new_key;
    return true;
  }

  Parser first;
  Parser last;
  std::string input_desc;
  size_t line_num;
  std::string key;
};

int MatchConstraintsToRules::Main(int argc, char *argv[]) {
  // Process command-line options.
  Options options;
  ProcessOptions(argc, argv, options);

  // Open the rule table index and constraint map streams.
  std::ifstream rule_table_index_stream;
  std::ifstream constraint_map_stream;
  OpenNamedInputOrDie(options.rule_table_index_file, rule_table_index_stream);
  OpenNamedInputOrDie(options.constraint_map_file, constraint_map_stream);

  // Open the output stream.
  std::ostream &output = OpenOutputOrDie(options.output_file);

  // Initialise the rule table index reader.
  InputReader<RuleTableIndexParser> rti_reader(
      RuleTableIndexParser(rule_table_index_stream),
      RuleTableIndexParser(),
      "rule table index file");

  // Initialise the constraint map reader.
  InputReader<ConstraintMapParser> cm_reader(
      ConstraintMapParser(constraint_map_stream),
      ConstraintMapParser(),
      "constraint map file");

  // Pair up entries with matching key values.
  while (rti_reader && cm_reader) {
    // Determine if keys match or otherwise, which file is 'ahead' of the other.
    int ret = rti_reader.key.compare(cm_reader.key);
    if (ret < 0) {
      rti_reader.ReadLine();
      continue;
    } else if (ret > 0) {
      cm_reader.ReadLine();
      continue;
    }
    // Keys match: output line number / ID set pair and advance rule table
    // index file.
    assert(ret == 0);
    output << rti_reader.first->line_num << " |||";
    const std::vector<StringPiece> &ids = cm_reader.first->ids;
    for (std::vector<StringPiece>::const_iterator p = ids.begin();
         p != ids.end(); ++p) {
      output << " " << *p;
    }
    output << std::endl;
    rti_reader.ReadLine();
  }

  return 0;
}

void MatchConstraintsToRules::ProcessOptions(int argc, char *argv[],
                                             Options &options) const {
  namespace po = boost::program_options;

  std::ostringstream usage_top;
  usage_top << "Usage: " << name() << " [OPTION]... RULE-TABLE-INDEX CONSTRAINT-MAP\n\n"
            << "Read a rule table index file and a constraint map file and perform a join on the\ntarget side symbols, writing the rule table line number and constraint ID set to\noutput.  The input files must be sorted using the \"C\" locale.\n\n"
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
    ("rti-file", po::value(&options.rule_table_index_file), "rule table index file")
    ("cm-file", po::value(&options.constraint_map_file), "constraint map file")
  ;

  // Compose the full set of command-line options.
  po::options_description cmd_line_options;
  cmd_line_options.add(visible).add(hidden);

  // Register the positional options.
  po::positional_options_description p;
  p.add("rti-file", 1);
  p.add("cm-file", 1);

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

  if (!vm.count("rti-file") || !vm.count("cm-file")) {
    std::ostringstream msg;
    msg << "missing required argument\n\n" << visible << usage_bottom.str()
        << std::endl;
    Error(msg.str());
  }
}

}  // namespace tool
}  // namespace taco
