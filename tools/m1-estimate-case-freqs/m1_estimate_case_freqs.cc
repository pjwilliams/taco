#include "m1_estimate_case_freqs.h"

#include "case_inferrer.h"
#include "options.h"
#include "tree_parser.h"
#include "typedef.h"

#include "tools-common/m1/case_model.h"
#include "tools-common/m1/st_relation.h"

#include "taco/feature_structure.h"
#include "taco/lexicon.h"
#include "taco/text-formats/feature_structure_parser.h"

#include <boost/program_options.hpp>

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace taco {
namespace tool {
namespace m1 {

int EstimateCaseFreqs::Main(int argc, char *argv[]) {
  // Process command-line options.
  Options options;
  ProcessOptions(argc, argv, options);

  // Open the input streams.
  std::ifstream corpus_stream;
  std::ifstream lexicon_stream;
  OpenNamedInputOrDie(options.corpus_file, corpus_stream);
  OpenNamedInputOrDie(options.lexicon_file, lexicon_stream);

  // Open the output stream.
  std::ostream &output = OpenOutputOrDie(options.output_file);

  Vocabulary lexicon_vocab;
  Vocabulary feature_set;
  Vocabulary value_set;

  FeatureStructureParser fs_parser(feature_set, value_set);

  // Load the lexicon.
  Lexicon<size_t> lexicon;
  BasicLexiconLoader lexicon_loader(fs_parser, lexicon_vocab);
  std::cerr << "Loading lexicon..." << std::endl;
  lexicon_loader.Load(lexicon_stream, lexicon);
  std::cerr << "Done" << std::endl;

  Feature infl_feature = feature_set.Insert("INFL");
  Feature case_feature = feature_set.Insert("CASE");
  Feature pos_feature = feature_set.Insert("CAT");
  CaseInferrer case_inferrer(lexicon, lexicon_vocab, value_set, infl_feature,
                             case_feature, pos_feature);

  typedef std::map<AtomicValue, float> InnerMap;
  typedef std::map<std::string, InnerMap> CountMap;

  CountMap counts;

  TreeParser parser(kSelectorTargetAttributeName);
  std::string line;
  size_t line_num = 0;
  while (std::getline(corpus_stream, line)) {
    ++line_num;
    std::auto_ptr<Tree> t(parser.Parse(line));
    if (!t.get()) {
      std::ostringstream msg;
      msg << "failed to parse tree at line " << line_num;
      Warn(msg.str());
      continue;
    }

    typedef std::map<int, Relation> RelationMap;
    typedef std::set<AtomicValue> CaseSet;

    RelationMap relations;

    ExtractRelations<Tree, kIdxId, RelationMap>(*t, relations);
    for (RelationMap::const_iterator p(relations.begin());
         p != relations.end(); ++p) {
      const Relation &relation = p->second;
      std::string key;
      FindNounPhrase(relation, options.tree_type, key);
      if (key.empty()) {
        continue;
      }
      CaseSet cases;
      try {
        case_inferrer.Infer(relation, cases);
      } catch (const Exception &e) {
        std::ostringstream msg;
        msg << "failed to infer case value at line " << line_num;
        msg << ": " << e.msg();
        // FIXME This is a warning and not an error because I need to fix
        // lexicon file format to allow an entry for '#'.  It should probably
        // be changed to error subsequently.
        Warn(msg.str());
        continue;
      }
      if (cases.empty()) {
        continue;
      }
      int num_cases = cases.size();
      if (num_cases > 1) {
        continue;
      }
      InnerMap &inner_map = counts[key];
      for (CaseSet::const_iterator q(cases.begin()); q != cases.end(); ++q) {
        AtomicValue case_val = *q;
        float count = 1.0 / num_cases;
        inner_map[case_val] += count;
      }
    }
  }

  // Print rules and counts
  for (CountMap::const_iterator p(counts.begin()); p != counts.end(); ++p) {
    const std::string &key = p->first;
    const InnerMap &inner_map = p->second;

    assert(!inner_map.empty());

    CaseTable::ProbabilityFunction probabilities;

    float total = 0.0;
    for (InnerMap::const_iterator q(inner_map.begin());
         q != inner_map.end(); ++q) {
      float count = q->second;
      total += count;
    }

    for (InnerMap::const_iterator q(inner_map.begin());
         q != inner_map.end(); ++q) {
      AtomicValue case_val = q->first;
      float count = q->second;
      probabilities[case_val] = count / total;
    }

    output << key << " |||";
    for (CaseTable::ProbabilityFunction::const_iterator p = probabilities.begin();
         p != probabilities.end(); ++p) {
      AtomicValue case_val = p->first;
      float prob = p->second;
      output << " " << value_set.Lookup(case_val)
             << ":" << std::setprecision(3) << std::fixed << prob;
    }
    output << " ||| " << std::setprecision(3) << std::fixed << total;
    output << std::endl;
  }

  return 0;
}

void EstimateCaseFreqs::ProcessOptions(int argc, char *argv[],
                                       Options &options) const {
  namespace po = boost::program_options;

  // Construct the 'top' of the usage message: the bit that comes before the
  // options list.
  std::ostringstream usage_top;
  usage_top << "Usage: " << name() << " [OPTION]... CORPUS LEXICON\n\n"
            << "Evaluate selector-target relations from CORPUS and estimate probability distribution\nover case values for each NP function label.\n\n"
            << "Options";

  // Construct the 'bottom' of the usage message.
  std::ostringstream usage_bottom;  // Empty for now.

  // Declare the command line options that are visible to the user.
  po::options_description visible(usage_top.str());
  visible.add_options()
    ("help",
        "print this help message and exit")
    ("output,o",
        po::value(&options.output_file),
        "write to arg instead of standard output")
    ("tree-type",
        po::value<std::string>(),
        "one of: bitpar (default), parzu")
  ;

  // Declare the command line options that are hidden from the user
  // (these are used as positional options).
  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("corpus-file",
        po::value(&options.corpus_file),
        "corpus file")
    ("lexicon-file",
        po::value(&options.lexicon_file),
        "lexicon file")
  ;

  // Compose the full set of command-line options.
  po::options_description cmd_line_options;
  cmd_line_options.add(visible).add(hidden);

  // Register the positional options.
  po::positional_options_description p;
  p.add("corpus-file", 1);
  p.add("lexicon-file", 1);

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

  // Process positional options.
  if (!vm.count("corpus-file") || !vm.count("lexicon-file")) {
    std::ostringstream msg;
    msg << "missing required argument\n\n" << visible << usage_bottom.str()
        << std::endl;
    Error(msg.str());
  }

  // Process remaining options.
  if (vm.count("tree-type")) {
    std::string arg = vm["tree-type"].as<std::string>();
    if (!StrToParseTreeType(arg, options.tree_type)) {
      std::string msg = "Unknown parse tree type: " + arg;
      Error(msg);
    }
  }
}

void EstimateCaseFreqs::FindNounPhrase(const Relation &relation,
                                       ParseTreeType tree_type,
                                       std::string &key) const {
  for (Relation::ConstIterator p = relation.Begin();
       p != relation.End(); ++p) {
    const Tree *tree = *p;
    if (tree->IsLeaf() || tree->IsPreterminal()) {
      continue;
    }
    if (tree_type == kBitPar) {
      de::BitParLabel label;
      label_parser_.Parse(tree->label().get<kIdxCat>(), label);
      if (label.cat == "NP") {
        key = label.func;
        break;
      }
    } else if (tree_type == kParZu) {
      const std::string &cat = tree->label().get<kIdxCat>();
      if (cat == "subj" || cat == "obja" || cat == "objd" || cat == "objg" ||
          cat == "pred" || cat == "gmod"  || cat == "gmod_pre" ||
          cat == "gmod_post" || cat == "zeit" || cat == "kon_gmod" ||
          cat == "kon_gmod_post"  || cat == "kon_gmod_pre" ||
          cat == "kon_obja"  || cat == "kon_objd"  || cat == "kon_objg"  ||
          cat == "kon_pp" || cat == "kon_objp" || cat == "kon_subj" ||
          cat == "kon_pred" || cat == "kon_zeit" || cat == "root" ||
          cat == "kon_root") {
        key = cat;
        break;
      }
    } else {
      assert(false);
    }
  }
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
