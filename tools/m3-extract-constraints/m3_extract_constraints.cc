#include "m3_extract_constraints.h"

#include "extractor.h"
#include "options.h"
#include "tree_context.h"
#include "tree_parser.h"
#include "writer.h"

#include "tools-common/m1/case_model.h"
#include "tools-common/m3/st_relation.h"
#include "tools-common/relation/relation_tree_ops.h"
#include "tools-common/text-formats/constraint_extract_writer.h"

#include "taco/constraint_set_set.h"
#include "taco/text-formats/constraint_writer.h"
#include "taco/base/vocabulary.h"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace taco {
namespace tool {
namespace m3 {

int ExtractConstraints::Main(int argc, char *argv[]) {
  // Process command-line options.
  Options options;
  ProcessOptions(argc, argv, options);

  // Open the corpus and target rule streams.
  std::ifstream corpus_stream;
  std::ifstream rule_stream;
  OpenNamedInputOrDie(options.corpus_file, corpus_stream);
  OpenNamedInputOrDie(options.rule_file, rule_stream);

  // Open the output stream.
  std::ostream &output = OpenOutputOrDie(options.output_file);

  Vocabulary vocab;
  Vocabulary feature_set;
  Vocabulary value_set;

  // Load case table if requested.
  std::auto_ptr<m1::CaseTable> case_table;
  if (!options.case_table_file.empty()) {
    case_table.reset(new m1::CaseTable());
    std::ifstream input;
    OpenNamedInputOrDie(options.case_table_file, input);
    m1::CaseTableLoader loader(vocab, value_set);
    std::cerr << "Loading case table..." << std::endl;
    loader.Load(input, *case_table);
    std::cerr << "Done..." << std::endl;
  }

  Extractor extractor(feature_set, value_set, options);
  TreeContext tree_context(case_table.get());

  CSVec constraint_sets;
  ConstraintWriter constraint_writer(feature_set, value_set);
  ConstraintExtractWriter extract_writer(constraint_writer, output);
  Writer writer(extract_writer);
  std::vector<std::size_t> node_indices;
  TreeFragment fragment;
  std::vector<Tree *> tree_nodes;
  std::auto_ptr<Tree> tree;
  TreeParser parser(kSelectorTargetAttributeName);
  int curr_tree_num = 0;  // Trees are indexed from 1 in rule file.
  size_t line_num = 0;
  std::string rule_line;
  std::string tree_line;
  while (std::getline(rule_stream, rule_line)) {
    ++line_num;

    int required_tree_num;
    try {
      ParseLine(rule_line, required_tree_num, node_indices);
    } catch (const Exception &e) {
      std::ostringstream msg;
      msg << "failed to parse rule file at line " << line_num
          << ": " << e.msg();
      Error(msg.str());
    }

    // Check if a new tree needs to be read.
    while (required_tree_num > curr_tree_num) {
      std::getline(corpus_stream, tree_line);
      if (++curr_tree_num < required_tree_num) {
        continue;
      }
      tree = parser.Parse(tree_line);
      if (!tree.get()) {
        std::ostringstream msg;
        msg << "failed to parse tree at line " << curr_tree_num;
        Error(msg.str());
      }
      tree_nodes.clear();
      EnumerateNodes(*tree, tree_nodes);
      tree_context.ResetTree(*tree);
    }

    // Fill fragment using node_indices and tree_nodes.
    std::size_t len = node_indices.size();
    fragment.leaves.clear();
    fragment.leaves.reserve(len-1);
    for (std::size_t i = 0; i < len; ++i) {
      std::size_t index = node_indices[i];
      if (index >= tree_nodes.size()) {
        std::ostringstream msg;
        msg << "line " << line_num
            << ": rule index out of bounds: " << index
            << " exceeds max tree node index ("
            << tree_nodes.size()-1
            << ")";
        Error(msg.str());
      }
      if (i == len-1) {
        fragment.root = tree_nodes[index];
      } else {
        fragment.leaves.push_back(tree_nodes[index]);
      }
    }

    // Extract a CSVec from fragment.
    try {
      extractor.Extract(fragment, tree_context, constraint_sets);
    } catch (const Exception &e) {
      std::ostringstream msg;
      msg << "failed to extract constraints at line " << line_num
          << ": " << e.msg();
      Error(msg.str());
    }

    // Write rule and constraint sets to output.
    writer.Write(fragment, constraint_sets);
  }

  return 0;
}

void ExtractConstraints::ProcessOptions(int argc, char *argv[],
                                        Options &options) const {
  namespace po = boost::program_options;

  // Construct the 'top' of the usage message: the bit that comes before the
  // options list.
  std::ostringstream usage_top;
  usage_top << "Usage: " << name() << " [OPTION]... CORPUS INDICES\n\n"
            << "Read target-side rules from INDICES and extract constraints.\n\n"
            << "Options";

  // Construct the 'bottom' of the usage message.
  std::ostringstream usage_bottom;  // Empty for now.

  // Declare the command line options that are visible to the user.
  po::options_description visible(usage_top.str());
  visible.add_options()
    ("help",
        "print this help message and exit")
    ("case-model",
        po::value(&options.case_table_file),
        "table for case model")
    ("case-model-hc",
        po::value(&options.case_model_threshold),
        "hard case constraint with arg as threshold probability")
    ("map-cat-values",
        "map CAT values from GDT tag set to analyser tag set")
    ("no-cat",
        "do not produce CAT constraints")
    ("output,o",
        po::value(&options.output_file),
        "write to arg instead of standard output")
    ("retain-lexical",
        "retain purely lexical constraint sets")
  ;

  // Declare the command line options that are hidden from the user
  // (these are used as positional options).
  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("corpus-file",
        po::value(&options.corpus_file),
        "corpus file")
    ("rule-file",
        po::value(&options.rule_file),
        "rule file")
  ;

  // Compose the full set of command-line options.
  po::options_description cmd_line_options;
  cmd_line_options.add(visible).add(hidden);

  // Register the positional options.
  po::positional_options_description p;
  p.add("corpus-file", 1);
  p.add("rule-file", 1);

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
  if (!vm.count("corpus-file") || !vm.count("rule-file")) {
    std::ostringstream msg;
    msg << "missing required argument\n\n" << visible << usage_bottom.str()
        << std::endl;
    Error(msg.str());
  }

  // Process remaining options.
  if (vm.count("retain-lexical")) {
    options.retain_lexical = true;
  }
  if (vm.count("map-cat-values")) {
    options.map_cat_values = true;
  }
  if (vm.count("no-cat")) {
    options.no_cat = true;
  }
}

// Perform depth-first enumeration of tree rooted at 'root,' appending
// nodes to 'tree_nodes.'
void ExtractConstraints::EnumerateNodes(Tree &root,
                                        std::vector<Tree *> &tree_nodes) const {
  std::vector<Tree *> &children = root.children();
  for (std::vector<Tree *>::const_iterator p = children.begin();
       p != children.end(); ++p) {
    Tree &child = **p;
    EnumerateNodes(child, tree_nodes);
  }
  tree_nodes.push_back(&root);
}

void ExtractConstraints::ParseLine(
    const std::string &line,
    int &tree_number,
    std::vector<std::size_t> &node_indices) const {
  const char *c_line = line.c_str();

  // Extract tree_number.
  tree_number = std::atoi(c_line);

  std::size_t pos = line.find("|||");
  if (pos == std::string::npos) {
    throw Exception("Missing column delimiter");
  }

  // Extract node_indices.
  node_indices.clear();
  c_line += pos+3;
  char *end_ptr;
  while (true) {
    long i = std::strtol(c_line, &end_ptr, 10);
    if (end_ptr == c_line) {
      break;
    }
    node_indices.push_back(static_cast<std::size_t>(i));
    c_line = end_ptr;
  }
  if (node_indices.size() < 2) {
    std::ostringstream msg;
    msg << "Expected 2 or more node indices, got " << node_indices.size();
    throw Exception(msg.str());
  }
}

}  // namespace m3
}  // namespace tool
}  // namespace taco
