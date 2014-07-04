#include "add_constraint_ids.h"

#include "options.h"

#include <boost/program_options.hpp>

#include <cstdlib>
#include <fstream>
#include <sstream>

namespace taco {
namespace tool {

int AddConstraintIds::Main(int argc, char *argv[]) {
  // Process command-line options.
  Options options;
  ProcessOptions(argc, argv, options);

  // Open the rule table and join file streams.
  std::ifstream rule_table_stream;
  std::ifstream join_stream;
  OpenNamedInputOrDie(options.rule_table_file, rule_table_stream);
  OpenNamedInputOrDie(options.join_file, join_stream);

  // Open the output stream.
  std::ostream &output = OpenOutputOrDie(options.output_file);

  std::string rule_table_line;
  std::string join_line;

  int curr_rule_num = 0;
  int prev_required_rule_num = -1;
  while (std::getline(join_stream, join_line)) {
    size_t pos = join_line.find("|||");
    // TODO Proper error handling.
    assert(pos != std::string::npos);
    int required_rule_num = std::atoi(join_line.c_str());
    assert(required_rule_num > prev_required_rule_num);
    while (true) {
      std::getline(rule_table_stream, rule_table_line);
      // TODO Proper error handling.
      assert(rule_table_stream);
      if (++curr_rule_num == required_rule_num) {
        output << rule_table_line << " " << join_line.substr(pos) << std::endl;
        break;
      } else {
        output << rule_table_line << " |||" << std::endl;
      }
    }
    prev_required_rule_num = required_rule_num;
  }

  return 0;
}

void AddConstraintIds::ProcessOptions(int argc, char *argv[],
                                      Options &options) const {
  namespace po = boost::program_options;

  std::ostringstream usage_top;
  usage_top << "usage: " << name() << " RULE-TABLE JOIN-FILE\n\n"
            << "Read a rule table and a rule-constraint join file...\n\n"
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
    ("join-file", po::value(&options.join_file), "join file")
  ;

  // Compose the full set of command-line options.
  po::options_description cmd_line_options;
  cmd_line_options.add(visible).add(hidden);

  // Register the positional options.
  po::positional_options_description p;
  p.add("rule-table-file", 1);
  p.add("join-file", 1);

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

  if (!vm.count("rule-table-file") || !vm.count("join-file")) {
    std::ostringstream msg;
    msg << "missing required argument\n\n" << visible << usage_bottom.str() << std::endl;
    Error(msg.str());
  }
}

}  // namespace tool
}  // namespace taco
