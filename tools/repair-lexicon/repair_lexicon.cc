#include "repair_lexicon.h"

#include "options.h"

#include "taco/base/exception.h"
#include "taco/lexicon.h"
#include "taco/text-formats/feature_structure_writer.h"
#include "taco/text-formats/lexicon_parser.h"
#include "taco/base/vocabulary.h"

#include <boost/program_options.hpp>
#include <boost/unordered_set.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace taco {
namespace tool {

int RepairLexicon::Main(int argc, char *argv[]) {
  // Process command-line options.
  Options options;
  ProcessOptions(argc, argv, options);

  // Open the input stream.
  std::istream &input = OpenInputOrDie(options.input_file);

  // Open the output stream.
  std::ostream &output = OpenOutputOrDie(options.output_file);

  Vocabulary vocabulary;
  Vocabulary feature_set;
  Vocabulary value_set;

  FeatureStructureParser fs_parser(feature_set, value_set);

  // Read the replacement lexicon if given.
  Lexicon<std::size_t> replacement_lexicon;
  if (!options.replace_file.empty()) {
    std::ifstream replace_stream;
    OpenNamedInputOrDie(options.replace_file, replace_stream);
    BasicLexiconLoader loader(fs_parser, vocabulary);
    loader.Load(replace_stream, replacement_lexicon);
  }

  FeatureStructureWriter fs_writer(feature_set, value_set);
  BasicLexiconWriter lexicon_writer(vocabulary, fs_writer);

  LexiconParser end;
  boost::unordered_set<std::string> replaced_words;
  for (LexiconParser parser(input); parser != end; ++parser) {
    typedef Lexicon<std::size_t>::MappedType Entries;
    const LexiconParser::Entry &entry = *parser;
    if (replacement_lexicon.IsEmpty()) {
      lexicon_writer.WriteLine(entry.word, entry.fs, output);
      continue;
    }
    if (replaced_words.find(entry.word) != replaced_words.end()) {
      continue;
    }
    std::size_t word_id = vocabulary.Insert(entry.word);
    const Entries *entries = replacement_lexicon.Lookup(word_id);
    if (entries == 0) {
      lexicon_writer.WriteLine(entry.word, entry.fs, output);
      continue;
    }
    for (Entries::const_iterator p = entries->begin(); p != entries->end();
         ++p) {
      lexicon_writer.WriteLine(entry.word, **p, output);
    }
    replaced_words.insert(entry.word);
  }

  return 0;
}

void RepairLexicon::ProcessOptions(int argc, char *argv[],
                                   Options &options) const {
  namespace po = boost::program_options;

  // Construct the 'top' of the usage message: the bit that comes before the
  // options list.
  std::ostringstream usage_top;
  usage_top << "Usage: " << name() << " [OPTION]... [FILE]\n\n"
            << "TODO.  With no FILE argument, or when FILE is -, read standard\ninput.\n\n"
            << "Options";

  // Construct the 'bottom' of the usage message.
  std::ostringstream usage_bottom;  // Empty for now.

  // Declare the command line options that are visible to the user.
  po::options_description visible(usage_top.str());
  visible.add_options()
    ("help",
        "print this help message and exit")
    ("replace-file,r",
        po::value(&options.replace_file),
        "replace file")
    ("output,o",
        po::value(&options.output_file),
        "write to arg instead of standard output")
  ;

  // Declare the command line options that are hidden from the user
  // (these are used as positional options).
  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("input",
        po::value(&options.input_file),
        "input file")
  ;

  // Compose the full set of command-line options.
  po::options_description cmd_line_options;
  cmd_line_options.add(visible).add(hidden);

  // Register the positional options.
  po::positional_options_description p;
  p.add("input", 1);

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
}

}  // namespace tool
}  // namespace taco
