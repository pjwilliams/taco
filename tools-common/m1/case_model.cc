#include "tools-common/m1/case_model.h"

#include "taco/base/exception.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <iomanip>
#include <istream>
#include <string>
#include <vector>

namespace taco {
namespace tool {
namespace m1 {

const CaseTable::ProbabilityFunction *CaseTable::Lookup(const Key &key) const {
  const_iterator p = table_.find(key);
  return (p == table_.end()) ? 0 : &(p->second);
}

void CaseTable::Insert(const Key &key, const ProbabilityFunction &probs) {
  table_[key] = probs;
}

void CaseTableLoader::Load(std::istream &tableStream, CaseTable &table) const {
  CaseTableParser end;
  for (CaseTableParser parser(tableStream); parser != end; ++parser) {
    const CaseTableParser::Entry &entry = *parser;

    // Construct the probability function.
    CaseTable::ProbabilityFunction probabilities;
    for (std::map<std::string, float>::const_iterator
         p(entry.probabilities.begin()); p != entry.probabilities.end(); ++p) {
      const std::string &case_string = p->first;
      float probability = p->second;
      AtomicValue case_val = value_set_.Insert(case_string);
      probabilities[case_val] = probability;
    }
    table.Insert(entry.func, probabilities);
  }
}

void CaseTableWriter::Write(const CaseTable &table, std::ostream &out) const {
  CaseTable::const_iterator end = table.end();
  for (CaseTable::const_iterator p = table.begin(); p != end; ++p) {
    const CaseTable::Key &key = p->first;
    const CaseTable::ProbabilityFunction &probs = p->second;
    WriteLine(key, probs, out);
  }
}

void CaseTableWriter::WriteLine(const CaseTable::Key &key,
                                const CaseTable::ProbabilityFunction &probs,
                                std::ostream &out,
                                float count) const {
  out << key << " |||";
  for (CaseTable::ProbabilityFunction::const_iterator p = probs.begin();
       p != probs.end(); ++p) {
    AtomicValue case_val = p->first;
    float prob = p->second;
    out << " " << value_set_.Lookup(case_val)
        << ":" << std::setprecision(3) << std::fixed << prob;
  }
  if (count > 0.0f) {
    out << " ||| " << std::setprecision(3) << std::fixed << count;
  }
  out << std::endl;
}

CaseTableParser &CaseTableParser::operator++() {
  if (!input_) {
    return *this;
  }
  std::string line;
  if (!std::getline(*input_, line)) {
    input_ = 0;
    return *this;
  }
  ParseLine(line);
  return *this;
}

CaseTableParser CaseTableParser::operator++(int) {
  CaseTableParser tmp(*this);
  ++(*this);
  return tmp;
}

void CaseTableParser::ParseLine(const std::string &line) {
  // Grammatical function
  size_t pos = line.find("|||");
  if (pos == std::string::npos) {
    throw Exception("missing first delimiter");
  }
  value_.func = line.substr(0, pos);
  boost::trim(value_.func);

  // Probability distribution
  size_t begin = pos+3;
  pos = line.find("|||", begin);
  if (pos == std::string::npos) {
    throw Exception("missing second delimiter");
  }
  std::string prob_string = line.substr(begin, pos-begin);
  boost::trim(prob_string);
  std::vector<std::string> pairs;
  boost::split(pairs, prob_string, boost::algorithm::is_space(),
               boost::algorithm::token_compress_on);
  value_.probabilities.clear();
  for (std::vector<std::string>::const_iterator p(pairs.begin());
       p != pairs.end(); ++p) {
    const std::string &pair = *p;
    size_t colon_pos = pair.rfind(':');
    // TODO Handle error if missing.
    std::string case_string = pair.substr(0, colon_pos);
    std::string prob_string = pair.substr(colon_pos+1);
    float prob = boost::lexical_cast<float>(prob_string);
    // TODO Handle error if can't convert.
    value_.probabilities[case_string] = prob;
  }

  // Count
  std::string count_text = line.substr(pos+3);
  boost::trim(count_text);
  value_.count = boost::lexical_cast<float>(count_text);
  // TODO Handle error if can't convert.
}

bool operator==(const CaseTableParser &lhs, const CaseTableParser &rhs) {
  // TODO Is this right?  Compare values of istreams if non-zero?
  return lhs.input_ == rhs.input_;
}

bool operator!=(const CaseTableParser &lhs, const CaseTableParser &rhs) {
  return !(lhs == rhs);
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
