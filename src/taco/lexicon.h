#ifndef TACO_SRC_TACO_LEXICON_H_
#define TACO_SRC_TACO_LEXICON_H_

#include <istream>
#include <ostream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "taco/base/exception.h"
#include "taco/feature_structure.h"
#include "taco/text-formats/feature_structure_parser.h"
#include "taco/text-formats/feature_structure_writer.h"
#include "taco/text-formats/lexicon_parser.h"

namespace taco {

template<typename K=size_t>
class Lexicon {
 public:
  typedef K KeyType;
  typedef std::vector<boost::shared_ptr<FeatureStructure> > MappedType;

 private:
  typedef boost::unordered_map<size_t, MappedType> Map;

 public:
  typedef Map::iterator Iterator;
  typedef Map::const_iterator ConstIterator;

  Iterator Begin() { return map_.begin(); }
  Iterator End() { return map_.end(); }

  ConstIterator Begin() const { return map_.begin(); }
  ConstIterator End() const { return map_.end(); }

  size_t Size() const { return map_.size(); }
  bool IsEmpty() const { return map_.empty(); }

  void Insert(const K &, boost::shared_ptr<FeatureStructure>);

  // Lookup the key and return a pointer to the MappedType if found and 0
  // otherwise.
  const MappedType *Lookup(const K &) const;

  // Lookup the key and then filter the mapped values according to whether
  // of not they can be unified with the given feature structure.  Pointers
  // to the unifiable values are copied to the output iterator.  Returns true
  // iff at least one unifiable value was found.
  template<typename OutputIterator>
  bool Lookup(const K &, const FeatureStructure &, OutputIterator) const;

 private:
  Map map_;
};

class BasicLexiconLoader {
 public:
  BasicLexiconLoader(FeatureStructureParser &fs_parser, Vocabulary &vocab)
      : fs_parser_(fs_parser)
      , vocabulary_(vocab) {}

  void Load(std::istream &, Lexicon<size_t> &);

 private:
  FeatureStructureParser &fs_parser_;
  Vocabulary &vocabulary_;
};

class BasicLexiconWriter {
 public:
  BasicLexiconWriter(const Vocabulary &vocab,
                     const FeatureStructureWriter &fs_writer)
      : vocabulary_(vocab)
      , fs_writer_(fs_writer) {}

  void Write(const Lexicon<size_t> &, std::ostream &) const;

  void WriteLine(const std::string &, const std::string &,
                 std::ostream &) const;

  void WriteLine(const std::string &, const FeatureStructure &,
                 std::ostream &) const;

 private:
  const Vocabulary &vocabulary_;
  const FeatureStructureWriter &fs_writer_;
};

template<typename K>
const typename Lexicon<K>::MappedType *Lexicon<K>::Lookup(const K &k) const {
  Map::const_iterator p = map_.find(k);
  return (p == map_.end()) ? 0 : &(p->second);
}

template<typename K>
template<typename OutputIterator>
bool Lexicon<K>::Lookup(const K &k, const FeatureStructure &x,
                        OutputIterator result) const {
  Map::const_iterator p = map_.find(k);
  if (p == map_.end()) {
    return false;
  }
  bool ret_val = false;
  for (MappedType::const_iterator q = p->second.begin(); q != p->second.end();
       ++q) {
    boost::shared_ptr<FeatureStructure> y = *q;
    boost::shared_ptr<FeatureStructure> x2 = x.Clone();
    boost::shared_ptr<FeatureStructure> y2 = y->Clone();
    if (FeatureStructure::Unify(x2, y2)) {
      ret_val = true;
      *result++ = y;
    }
  }
  return ret_val;
}

template<typename K>
void Lexicon<K>::Insert(const K &k, boost::shared_ptr<FeatureStructure> fs) {
  map_[k].push_back(fs);
}

}  // namespace taco

#endif
