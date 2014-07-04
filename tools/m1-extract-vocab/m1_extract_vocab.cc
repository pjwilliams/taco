#include "m1_extract_vocab.h"

#include "options.h"

#include "tools-common/compat-moses/string_tree_parser.h"

#include "taco/base/exception.h"
#include "tools-common/syntax-tree/string_tree.h"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/unordered_map.hpp>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace taco {
namespace tool {
namespace m1 {

namespace {

typedef boost::unordered_map<std::string, std::size_t> PosCountMap;
typedef std::multimap<std::size_t, std::string> InvPosCountMap;

struct SymbolStats {
  SymbolStats() : total(0) {}
  PosCountMap pos_counts;
  int total;
};

// Symbol.second is a flag indicating if the symbol is a terminal (true) or
// a non-terminal(false).
typedef std::pair<std::string, bool> Symbol;

typedef boost::unordered_map<Symbol, SymbolStats> VocabMap;

typedef std::vector<const VocabMap::value_type *> VocabVec;

struct VocabVecOrderer {
 public:
  bool operator()(const VocabMap::value_type *a,
                  const VocabMap::value_type *b) {
    if (a->second.total == b->second.total) {
      return a->first < b->first;
    }
    return a->second.total > b->second.total;
  }
};

void BuildInversePosCountMap(const PosCountMap &pos_counts,
                             InvPosCountMap &inv_pos_counts) {
  inv_pos_counts.clear();
  for (PosCountMap::const_iterator p = pos_counts.begin();
       p != pos_counts.end(); ++p) {
    const std::string &pos = p->first;
    std::size_t count = p->second;
    inv_pos_counts.insert(std::make_pair(count, pos));
  }
}

void InsertSymbols(const StringTree &t, VocabMap &vocab_map) {
  Symbol symbol(t.label(), t.IsLeaf());
  SymbolStats &stats = vocab_map[symbol];
  ++stats.total;
  if (symbol.second) {
    ++stats.pos_counts[t.parent()->label()];
  }
  for (std::vector<StringTree *>::const_iterator p = t.children().begin();
       p != t.children().end(); ++p) {
    const StringTree &child = **p;
    InsertSymbols(child, vocab_map);
  }
}

}  // namespace

int ExtractVocab::Main(int argc, char *argv[]) {
  // Process command-line options.
  Options options;
  ProcessOptions(argc, argv, options);

  // Open the input stream.
  std::istream &input = OpenInputOrDie(options.input_file);

  // Open the output stream.
  std::ostream &output = OpenOutputOrDie(options.output_file);

  // Parse the input trees and build a map from symbols to stats.
  moses::StringTreeParser parser;
  VocabMap vocab_map;
  std::string line;
  size_t line_num = 0;
  while (std::getline(input, line)) {
    ++line_num;
    std::auto_ptr<StringTree> tree;
    try {
      tree = parser.Parse(line);
    } catch (const Exception &e) {
      std::ostringstream msg;
      msg << "failed to parse tree at line " << line_num << ": " << e.msg();
      Error(msg.str());
    }
    if (!tree.get()) {
      std::ostringstream msg;
      msg << "no tree at line " << line_num;
      Warn(msg.str());
      continue;
    }
    InsertSymbols(*tree, vocab_map);
  }

  // Sort the map's (symbol, stat) pairs by total.
  std::vector<const VocabMap::value_type *> vocab_vec;
  vocab_vec.reserve(vocab_map.size());
  for (VocabMap::const_iterator p = vocab_map.begin();
       p != vocab_map.end(); ++p) {
    const VocabMap::value_type &value = *p;
    vocab_vec.push_back(&value);
  }
  std::sort(vocab_vec.begin(), vocab_vec.end(), VocabVecOrderer());

  // Print the symbols and stats.
  InvPosCountMap inv_pos_counts;
  for (VocabVec::const_iterator p = vocab_vec.begin();
       p != vocab_vec.end(); ++p) {
    const VocabMap::value_type *value = *p;
    const Symbol &symbol = value->first;
    const SymbolStats &stats = value->second;
    if (symbol.second) {
      assert(!stats.pos_counts.empty());
      output << symbol.first;
    } else {
      assert(stats.pos_counts.empty());
      output << "[" << symbol.first << "]";
    }
    output << " ||| " << stats.total << " |||";
    BuildInversePosCountMap(stats.pos_counts, inv_pos_counts);
    for (InvPosCountMap::const_reverse_iterator q = inv_pos_counts.rbegin();
         q != inv_pos_counts.rend(); ++q) {
      output << " " << q->second;
    }
    output << " |||";
    for (InvPosCountMap::const_reverse_iterator q = inv_pos_counts.rbegin();
         q != inv_pos_counts.rend(); ++q) {
      output << " " << q->first;
    }
    output << std::endl;
  }

  return 0;
}

void ExtractVocab::ProcessOptions(int argc, char *argv[],
                                  Options &options) const {
  namespace po = boost::program_options;

  // Construct the 'top' of the usage message: the bit that comes before the
  // options list.
  std::ostringstream usage_top;
  usage_top << "Usage: " << name() << " [OPTION]... [FILE]\n\n"
            << "Read trees from input then list every distinct symbol and its count.  For each\nterminal, also list the set of observed preterminal labels.  Entries are output\nin rank order.  With no FILE argument, or when FILE is -, read standard input.\n\n"
            << "Options";

  // Construct the 'bottom' of the usage message.
  std::ostringstream usage_bottom;
  usage_bottom << "\nExamples:\n\n"
               << "1. ...";
  // TODO  Add some examples

  // Declare the command line options that are visible to the user.
  po::options_description visible(usage_top.str());
  visible.add_options()
    ("help",
        "print this help message and exit")
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

}  // namespace m1
}  // namespace tool
}  // namespace taco
