#include "m3_label_st_sets.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>

#include <boost/program_options.hpp>

#include "tools-common/m3/st_relation.h"
#include "tools-common/relation/relation.h"
#include "tools-common/relation/relation_tree_ops.h"

#include "extractor.h"
#include "options.h"
#include "tree_parser.h"
#include "tree_writer.h"
#include "typedef.h"

namespace taco {
namespace tool {
namespace m3 {

int LabelSTSets::Main(int argc, char *argv[]) {
  // Process command-line options.
  Options options;
  ProcessOptions(argc, argv, options);

  // Open the input and output streams.
  std::istream &input = OpenInputOrDie(options.input_file);
  std::ostream &output = OpenOutputOrDie(options.output_file);

  Extractor extractor(options);

  TreeParser parser(kSelectorTargetAttributeName);
  TreeWriter writer(kSelectorTargetAttributeName);

  std::string line;
  std::size_t line_num = 0;
  RelationSet relations;
  std::size_t total_relations = 0;
  std::vector<std::string> warnings;
  RelationMap relation_map;

  while (std::getline(input, line)) {
    ++line_num;
    // Parse the tree.
    std::auto_ptr<Tree> t(parser.Parse(line));
    if (!t.get()) {
      std::ostringstream msg;
      msg << "failed to parse tree at line " << line_num;
      Warn(msg.str());
      output << std::endl;
      continue;
    }
    // Extract the relations.
    relations.clear();
    warnings.clear();
    try {
      extractor.Extract(*t, relations, warnings);
    } catch (const Exception &e) {
      std::ostringstream msg;
      msg << "line " << line_num << ": " << e.msg();
      Error(msg.str());
    }
    total_relations += relations.size();
    // Report any warning messages.
    for (std::vector<std::string>::const_iterator p = warnings.begin();
         p != warnings.end(); ++p) {
      std::ostringstream msg;
      msg << "line " << line_num << ": " << *p;
      Warn(msg.str());
    }
    // Assign numeric IDs to the relations and build a map from ID to relation.
    NumberRelations(*t, relations, relation_map);
    // Set the relation IDs on the tree labels.
    SetRelationIds<Tree, kIdxId, const Relation*>(*t, relation_map);
    writer.Write(*t, output);
  }

  std::cerr << "Total relations extracted: " << total_relations << std::endl;

  return 0;
}

void LabelSTSets::ProcessOptions(int argc, char *argv[],
                                    Options &options) const {
  namespace po = boost::program_options;

  // Construct the 'top' of the usage message: the bit that comes before the
  // options list.
  std::ostringstream usage_top;
  usage_top << "Usage: " << name() << " [OPTION]... [FILE] \n\n"
            << "Read Greek parse trees from input, identify relations, and write labelled trees\nto output.  With no FILE argument, or when FILE is -, read standard input.\n\n"
            << "Options";

  // Construct the 'bottom' of the usage message.
  std::ostringstream usage_bottom;  // Empty for now.

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
    ("input-file",
        po::value(&options.input_file),
        "input file")
  ;

  // Compose the full set of command-line options.
  po::options_description cmd_line_options;
  cmd_line_options.add(visible).add(hidden);

  // Register the positional options.
  po::positional_options_description p;
  p.add("input-file", 1);

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

}  // namespace m3
}  // namespace tool
}  // namespace taco
