#include "tools-common/compat-moses/rule_table_parser.h"

#include "taco/base/exception.h"
#include "taco/base/string_util.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <istream>
#include <string>

namespace taco {
namespace tool {
namespace moses {

RuleTableParser::RuleTableParser()
    : input_(0) {
}

RuleTableParser::RuleTableParser(std::istream &input, int fields)
    : fields_(fields)
    , input_(&input) {
  ++(*this);
}

RuleTableParser &RuleTableParser::operator++() {
  if (!input_) {
    return *this;
  }
  if (!std::getline(*input_, value_.line)) {
    input_ = 0;
    return *this;
  }
  ParseLine(value_.line);
  return *this;
}

/*
RuleTableParser RuleTableParser::operator++(int) {
  RuleTableParser tmp(*this);
  ++(*this);
  return tmp;
}
*/

void RuleTableParser::ParseLine(StringPiece line) {

  // Source symbols
  std::size_t pos = line.find("|||");
  if (pos == std::string::npos) {
    throw Exception("missing first delimiter");
  }
  if ((fields_ & SOURCE_LHS) || (fields_ & SOURCE_RHS)) {
    StringPiece col = line.substr(0, pos);
    value_.source_rhs.clear();
    Tokenize(value_.source_rhs, col);
    if (fields_ & SOURCE_LHS) {
      value_.source_lhs = value_.source_rhs.back();
    }
    if (fields_ & SOURCE_RHS) {
      value_.source_rhs.pop_back();
      std::for_each(value_.source_rhs.begin(), value_.source_rhs.end(),
                    TrimPairedSymbolFromRight);
    }
  }

  // Target symbols
  std::size_t begin = pos+3;
  pos = line.find("|||", begin);
  if (pos == std::string::npos) {
    throw Exception("missing second delimiter");
  }
  if ((fields_ & TARGET_LHS) || (fields_ & TARGET_RHS)) {
    StringPiece col = line.substr(begin, pos-begin);
    value_.target_rhs.clear();
    Tokenize(value_.target_rhs, col);
    if (fields_ & TARGET_LHS) {
      value_.target_lhs = value_.target_rhs.back();
    }
    if (fields_ & TARGET_RHS) {
      value_.target_rhs.pop_back();
      std::for_each(value_.target_rhs.begin(), value_.target_rhs.end(),
                    TrimPairedSymbolFromLeft);
    }
  }

  int field_num = 2;
  begin = pos+3;

  // Scores
  begin = ParseField(line, begin, ++field_num, true, (fields_ & SCORES),
                     value_.scores);

  // Alignments
  begin = ParseField(line, begin, ++field_num, true, (fields_ & ALIGNMENT),
                     &SplitAlignmentPair, value_.alignment);

  // The counts field is the last obligatory field.  Any remaining fields are
  // optional unless the CONSTRAINT_IDS or FEATURE_SELECTION_ID flags is set,
  // in which case all fields are required to be present.
  bool require_all_fields = (fields_ & (CONSTRAINT_IDS | FEATURE_SELECTION_ID));

  // Counts
  begin = ParseField(line, begin, ++field_num, require_all_fields,
                     (fields_ & COUNTS), value_.counts);
  if (begin == std::string::npos) {
    assert(!require_all_fields);
    return;
  }

  // Sparse features
  begin = ParseField(line, begin, ++field_num, require_all_fields,
                     (fields_ & SPARSE_FEATURES), value_.sparse_features);
  if (begin == std::string::npos) {
    assert(!require_all_fields);
    return;
  }

  // Key-value pairs
  begin = ParseField(line, begin, ++field_num, require_all_fields,
                     (fields_ & KEY_VALUE_PAIRS), value_.key_value_pairs);
  if (begin == std::string::npos) {
    assert(!require_all_fields);
    return;
  }

  // Constraint IDs
  begin = ParseField(line, begin, ++field_num, require_all_fields,
                     (fields_ & CONSTRAINT_IDS), &SplitConstraintId,
                     value_.constraint_ids);
  if (begin == std::string::npos) {
    assert(!require_all_fields);
    return;
  }

  // Feature selection ID
  begin = ParseField(line, begin, ++field_num, false,
                     (fields_ & FEATURE_SELECTION_ID),
                     value_.feature_selection_id);
  // If there are fields beyond the feature selection ID field then throw
  // an exception.  If this happens then this function probably needs updating.
  if (begin != std::string::npos) {
    throw Exception("unexpected delimiter after feature selection ID");
  }
}

std::size_t RuleTableParser::ParseField(StringPiece line, std::size_t begin,
                                        int field_num, bool require_delim,
                                        bool store, StringPiece &dest) {
  std::size_t pos = line.find("|||", begin);
  if (require_delim && pos == std::string::npos) {
    throw Exception("missing " + Ordinal(field_num) + " delimiter");
  }
  if (store) {
    if (pos == std::string::npos) {
      dest = line.substr(begin);
    } else {
      dest = line.substr(begin, pos-begin);
    }
    Trim(dest);
  }
  return pos == std::string::npos ? pos : pos+3;
}

std::size_t RuleTableParser::ParseField(StringPiece line, std::size_t begin,
                                        int field_num, bool require_delim,
                                        bool store,
                                        std::vector<StringPiece> &dest) {
  std::size_t pos = line.find("|||", begin);
  if (require_delim && pos == std::string::npos) {
    throw Exception("missing " + Ordinal(field_num) + " delimiter");
  }
  if (store) {
    StringPiece col;
    if (pos == std::string::npos) {
      col = line.substr(begin);
    } else {
      col = line.substr(begin, pos-begin);
    }
    dest.clear();
    Tokenize(dest, col);
  }
  return pos == std::string::npos ? pos : pos+3;
}

std::size_t RuleTableParser::ParseField(
    StringPiece line, std::size_t begin, int field_num, bool require_delim,
    bool store,
    void (*Split)(const StringPiece &, std::pair<StringPiece,StringPiece> &),
    std::vector<std::pair<StringPiece, StringPiece> > &dest) {
  std::size_t pos = line.find("|||", begin);
  if (require_delim && pos == std::string::npos) {
    throw Exception("missing " + Ordinal(field_num) + " delimiter");
  }
  if (store) {
    StringPiece col;
    if (pos == std::string::npos) {
      col = line.substr(begin);
    } else {
      col = line.substr(begin, pos-begin);
    }
    tmp_vec_.clear();
    Tokenize(tmp_vec_, col);
    dest.clear();
    dest.resize(tmp_vec_.size());
    std::vector<std::pair<StringPiece,StringPiece> >::iterator p = dest.begin();
    for (std::vector<StringPiece>::const_iterator q = tmp_vec_.begin();
         q != tmp_vec_.end(); ++p, ++q) {
      Split(*q, *p);
    }
  }
  return pos == std::string::npos ? pos : pos+3;
}

std::string RuleTableParser::Ordinal(int i) {
  if (i == 1) {
    return "first";
  } else if (i == 2) {
    return "second";
  } else if (i == 3) {
    return "third";
  } else if (i == 4) {
    return "fourth";
  } else if (i == 5) {
    return "fifth";
  } else if (i == 6) {
    return "sixth";
  } else if (i == 7) {
    return "seventh";
  } else if (i == 8) {
    return "eighth";
  } else if (i == 9) {
    return "ninth";
  } else {
    return "";
  }
}

void RuleTableParser::TrimPairedSymbolFromLeft(StringPiece &s) {
  std::size_t len = s.size();
  if (len < 2 || s[0] != '[' || s[len-1] != ']') {
    return;
  }
  std::size_t pos = s.find('[', 1);
  if (pos == std::string::npos) {
    std::ostringstream msg;
    msg << "malformed non-terminal pair: " << s;
    throw Exception(msg.str());
  }
  s = s.substr(pos);
}

void RuleTableParser::TrimPairedSymbolFromRight(StringPiece &s) {
  std::size_t len = s.size();
  if (len < 2 || s[0] != '[' || s[len-1] != ']') {
    return;
  }
  std::size_t pos = s.find('[', 1);
  if (pos == std::string::npos) {
    std::ostringstream msg;
    msg << "malformed non-terminal pair: " << s;
    throw Exception(msg.str());
  }
  s = s.substr(0, pos);
}

void RuleTableParser::SplitAlignmentPair(
    const StringPiece &s,
    std::pair<StringPiece,StringPiece> &pair) {
  std::size_t pos = s.find('-');
  if (pos == std::string::npos) {
    throw Exception("missing delimiter in alignment pair");
  }
  pair.first = s.substr(0, pos);
  pair.second = s.substr(pos+1);
}

void RuleTableParser::SplitConstraintId(
    const StringPiece &s,
    std::pair<StringPiece,StringPiece> &pair) {
  std::size_t pos = s.find(':');
  if (pos == std::string::npos) {
    throw Exception("missing delimiter in constraint ID");
  }
  pair.first = s.substr(0, pos);
  pair.second = s.substr(pos+1);
}

bool operator==(const RuleTableParser &lhs, const RuleTableParser &rhs) {
  return lhs.input_ == rhs.input_;
}

bool operator!=(const RuleTableParser &lhs, const RuleTableParser &rhs) {
  return !(lhs == rhs);
}

}  // namespace moses
}  // namespace tool
}  // namespace taco
