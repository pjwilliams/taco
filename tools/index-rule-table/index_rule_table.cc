#include "index_rule_table.h"

#include "options.h"

#include "tools-common/compat-moses/rule_table_parser.h"
#include "tools-common/text-formats/vocab_parser.h"

#include "taco/base/string_piece.h"
#include "taco/base/vocabulary.h"

#include <boost/program_options.hpp>

#include <iostream>
#include <sstream>

namespace taco {
namespace tool {

int IndexRuleTable::Main(int argc, char *argv[]) {
  using moses::RuleTableParser;

  // Process command-line options.
  Options options;
  ProcessOptions(argc, argv, options);

  // Open the input streams.
  std::ifstream table_stream;
  std::ifstream vocab_stream;
  OpenNamedInputOrDie(options.table_file, table_stream);
  OpenNamedInputOrDie(options.vocab_file, vocab_stream);

  // Open the output stream.
  std::ostream &output = OpenOutputOrDie(options.output_file);

  // Load the symbol set from the vocab file.
  Vocabulary symbol_set;
  VocabParser vocab_end;
  for (VocabParser p(vocab_stream); p != vocab_end; ++p) {
    const StringPiece &symbol = p->symbol;
    symbol_set.Insert(symbol.as_string());
  }

  size_t line_num = 0;
  RuleTableParser end;
  const int fields = RuleTableParser::TARGET_LHS | RuleTableParser::TARGET_RHS;
  for (RuleTableParser parser(table_stream, fields); parser != end; ++parser) {
    const RuleTableParser::Entry &entry = *parser;
    ++line_num;
    output << symbol_set.Lookup(entry.target_lhs.as_string());
    for (std::vector<StringPiece>::const_iterator p = entry.target_rhs.begin();
         p != entry.target_rhs.end(); ++p) {
      output << "-" << symbol_set.Lookup(p->as_string());
    }
    output << " ||| " << line_num << std::endl;
  }

  return 0;
}

void IndexRuleTable::ProcessOptions(int argc, char *argv[],
                                    Options &options) const {
  namespace po = boost::program_options;

  std::ostringstream usage_top;
  usage_top << "Usage: " << name() << " [OPTION]... TABLE VOCAB\n\n"
            << "Read rule table from TABLE and index by target side.\n\n"
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
    ("table-file",
        po::value(&options.table_file),
        "rule table file")
    ("vocab-file",
        po::value(&options.vocab_file),
        "vocab file")
  ;

  // Compose the full set of command-line options.
  po::options_description cmd_line_options;
  cmd_line_options.add(visible).add(hidden);

  // Register the positional options.
  po::positional_options_description p;
  p.add("table-file", 1);
  p.add("vocab-file", 1);

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

  if (!vm.count("table-file") || !vm.count("vocab-file")) {
    std::ostringstream msg;
    msg << "missing required argument\n\n" << visible << usage_bottom.str()
        << std::endl;
    Error(msg.str());
  }
}

}  // namespace tool
}  // namespace taco
