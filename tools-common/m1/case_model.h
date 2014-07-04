#ifndef TACO_TOOLS_COMMON_M1_CASE_MODEL_H_
#define TACO_TOOLS_COMMON_M1_CASE_MODEL_H_

#include "taco/feature_structure.h"
#include "taco/base/vocabulary.h"

#include <boost/container/flat_map.hpp>
#include <boost/unordered_map.hpp>

#include <map>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

namespace taco {
namespace tool {
namespace m1 {

class CaseTable {
 public:
  typedef std::string Key;
  typedef boost::container::flat_map<AtomicValue, float> ProbabilityFunction;

 private:
  typedef boost::unordered_map<Key, ProbabilityFunction> Table;

 public:
  typedef Table::iterator iterator;
  typedef Table::const_iterator const_iterator;

  iterator begin() { return table_.begin(); }
  iterator end() { return table_.end(); }

  const_iterator begin() const { return table_.begin(); }
  const_iterator end() const { return table_.end(); }

  size_t Size() const { return table_.size(); }
  bool IsEmpty() const { return table_.empty(); }

  const ProbabilityFunction *Lookup(const Key &) const;

  void Insert(const Key &, const ProbabilityFunction &);

 private:
  Table table_;
};

// Parses streams in the CaseTable format.  Entries have a grammatical function
// tag, a probability distribution mapping case values to probabilities, and a
// count.  For example:
//
// OA ||| dat:0.062 nom:0.016 gen:0.007 acc:0.915 ||| 67686.000
//
class CaseTableParser {
 public:
  struct Entry {
    std::string func;
    std::map<std::string, float> probabilities;
    float count;
  };

  CaseTableParser() : input_(0) {}
  CaseTableParser(std::istream &input) : input_(&input) { ++(*this); }

  const Entry &operator*() const { return value_; }
  const Entry *operator->() const { return &value_; }

  CaseTableParser &operator++();
  CaseTableParser operator++(int);

  friend bool operator==(const CaseTableParser &, const CaseTableParser &);
  friend bool operator!=(const CaseTableParser &, const CaseTableParser &);

 private:
  void ParseLine(const std::string &);

  Entry value_;
  std::istream *input_;
};

class CaseTableLoader {
 public:
  CaseTableLoader(Vocabulary &/*vocab*/, Vocabulary &value_set)
      : value_set_(value_set) {}

  void Load(std::istream &, CaseTable &) const;

 private:
  Vocabulary &value_set_;
};

class CaseTableWriter {
 public:
  CaseTableWriter(const Vocabulary &/*non_term_set*/,
                  const Vocabulary &value_set)
      : value_set_(value_set) {}

  void Write(const CaseTable &, std::ostream &) const;

  void WriteLine(const CaseTable::Key &,
                 const CaseTable::ProbabilityFunction &,
                 std::ostream &,
                 float = 0.0f) const;

 private:
  const Vocabulary &value_set_;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
