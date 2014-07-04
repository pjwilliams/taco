#include "taco/lexicon.h"

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "taco/base/exception.h"
#include "taco/feature_structure.h"
#include "taco/text-formats/lexicon_parser.h"

namespace taco {

void BasicLexiconLoader::Load(std::istream &input, Lexicon<size_t> &lexicon) {
  // TODO Use boost::unordered_map
  std::map<FeatureStructureSpec,
           boost::shared_ptr<FeatureStructure> > spec_to_fs;

  LexiconParser end;
  FeatureStructureSpec spec;
  for (LexiconParser parser(input); parser != end; ++parser) {
    const LexiconParser::Entry &entry = *parser;
    size_t word_id = vocabulary_.Insert(entry.word);
    fs_parser_.Parse(entry.fs, spec);
    boost::shared_ptr<FeatureStructure> &fs = spec_to_fs[spec];
    if (!fs.get()) {
      fs.reset(new FeatureStructure(spec));
    }
    lexicon.Insert(word_id, fs);
  }
}

void BasicLexiconWriter::Write(const Lexicon<size_t> &lexicon,
                               std::ostream &output) const {
  Lexicon<size_t>::ConstIterator end = lexicon.End();
  for (Lexicon<size_t>::ConstIterator p = lexicon.Begin(); p != end; ++p) {
    size_t word_id = p->first;
    const Lexicon<size_t>::MappedType &fs_vec = p->second;
    const std::string &word = vocabulary_.Lookup(word_id);
    for (Lexicon<size_t>::MappedType::const_iterator q = fs_vec.begin();
         q != fs_vec.end(); ++q) {
      boost::shared_ptr<FeatureStructure> fs = *q;
      WriteLine(word, *fs, output);
    }
  }
}

void BasicLexiconWriter::WriteLine(const std::string &word,
                                   const FeatureStructure &fs,
                                   std::ostream &output) const {
  output << word;
  output << " ||| ";
  fs_writer_.Write(fs, output);
  output << std::endl;
}

void BasicLexiconWriter::WriteLine(const std::string &word,
                                   const std::string &fs,
                                   std::ostream &output) const {
  output << word << " ||| " << fs << std::endl;
}

}  // namespace taco
