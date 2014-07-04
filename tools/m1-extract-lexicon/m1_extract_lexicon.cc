#include "m1_extract_lexicon.h"

#include "interpreter.h"
#include "options.h"
#include "pos_table.h"

#include "taco/base/exception.h"
#include "taco/base/vocabulary.h"
#include "taco/lexicon.h"
#include "taco/text-formats/feature_structure_writer.h"

#include <boost/program_options.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace taco {
namespace tool {
namespace m1 {

int ExtractLexicon::Main(int argc, char *argv[]) {
  // Process command-line options.
  Options options;
  ProcessOptions(argc, argv, options);

  // Open the analysis and vocab streams.
  std::ifstream analysis_stream;
  std::ifstream vocab_stream;
  OpenNamedInputOrDie(options.analysis_file, analysis_stream);
  OpenNamedInputOrDie(options.vocab_file, vocab_stream);

  // Open the output stream.
  std::ostream &output = OpenOutputOrDie(options.output_file);

  Vocabulary vocabulary;
  Vocabulary feature_set;
  Vocabulary value_set;

  // Load the POS table from the vocab file.
  PosTable pos_table;
  PosTableLoader loader(vocabulary);
  try {
    loader.Load(vocab_stream, pos_table);
  } catch (const Exception &e) {
    std::ostringstream msg;
    msg << "failed to read vocab file: " << e.msg();
    Error(msg.str());
  }

  FeatureStructureWriter fs_writer(feature_set, value_set);
  BasicLexiconWriter lexicon_writer(vocabulary, fs_writer);

  Interpreter interpreter(options, vocabulary, feature_set, value_set,
                          pos_table, lexicon_writer, output);

  try {
    using de::smor::Parser;
    std::for_each(Parser(analysis_stream), Parser(), interpreter);
  } catch (const Exception &e) {
    Error(e.msg());
  }

  return 0;
}

void ExtractLexicon::ProcessOptions(int argc, char *argv[],
                                    Options &options) const {
  namespace po = boost::program_options;

  // Construct the 'top' of the usage message: the bit that comes before the
  // options list.
  std::ostringstream usage_top;
  usage_top << "Usage: " << name() << " [OPTION]... ANALYSES VOCAB\n\n"
            << "Read SMOR-format morphological analyses from ANALYSES and build a lexicon of\nfeature structures.  Use VOCAB to ensure that every word-POS pair in the corpus\nhas an entry in the lexicon (some feature structures may be empty).  With no\nFILE argument, or when FILE is -, read standard input.\n\n"
            << "Options";

  // Construct the 'bottom' of the usage message.
  std::ostringstream usage_bottom;  // Empty for now.

  // Declare the command line options that are visible to the user.
  po::options_description visible(usage_top.str());
  visible.add_options()
    ("help",
        "print this help message and exit")
    ("disable-decl-feature",
        "disable declension feature")
    ("disable-prep-case",
        "disable case feature for prepositions")
    ("output,o",
        po::value(&options.output_file),
        "write to arg instead of standard output")
  ;

  // Declare the command line options that are hidden from the user
  // (these are used as positional options).
  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("analysis-file",
        po::value(&options.analysis_file),
        "analysis file")
    ("vocab-file",
        po::value(&options.vocab_file),
        "vocab file")
  ;

  // Compose the full set of command-line options.
  po::options_description cmd_line_options;
  cmd_line_options.add(visible).add(hidden);

  // Register the positional options.
  po::positional_options_description p;
  p.add("analysis-file", 1);
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
  if (!vm.count("analysis-file") || !vm.count("vocab-file")) {
    std::ostringstream msg;
    msg << "missing required argument\n\n" << visible << usage_bottom.str()
        << std::endl;
    Error(msg.str());
  }

  // Process remaining options.
  if (vm.count("disable-decl-feature")) {
    options.disable_decl_feature = true;
  }
  if (vm.count("disable-prep-case")) {
    options.disable_prep_case = true;
  }
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
