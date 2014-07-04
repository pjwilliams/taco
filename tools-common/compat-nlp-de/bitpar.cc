#include "tools-common/compat-nlp-de/bitpar.h"

#include "taco/base/exception.h"

#include <boost/algorithm/string.hpp>

#include <sstream>

namespace taco {
namespace tool {
namespace de {

void BitParLabel::Clear() {
  cat.clear();
  func.clear();
  morph.clear();
  slash.clear();
}

void BitParLabelParser::Parse(const std::string &s, BitParLabel &label) const {
  label.Clear();
  parts_.clear();

  // Check for slash features.
  size_t i = s.find_first_of('/');
  if (i == std::string::npos) {
    boost::split(parts_, s, boost::algorithm::is_any_of("-"));
  } else {
    pre_slash_ = s.substr(0, i);
    slash_string_ = s.substr(i+1);
    boost::split(parts_, pre_slash_, boost::algorithm::is_any_of("-"));
    boost::split(label.slash, slash_string_, boost::algorithm::is_any_of("/"));
  }

  // Remove the numbers that appear in some labels.
  RemoveMysteriousNumbers();

  if (parts_.empty() || parts_.size() > 3) {
    std::ostringstream msg;
    msg << "expected 1-3 non-numeric label parts, found " << parts_.size();
    throw Exception(msg.str());
  }

  label.cat = parts_[0];
  if (parts_.size() == 1) {
    return;
  }
  label.func = parts_[1];
  if (parts_.size() == 2) {
    return;
  }
  boost::split(label.morph, parts_[2], boost::algorithm::is_any_of("."));
}

// There must be code in std or boost to do this...
bool BitParLabelParser::IsNumeric(const std::string &s) {
  if (s.empty()) {
    return false;
  }
  for (std::string::const_iterator p = s.begin(); p != s.end(); ++p) {
    if (!std::isdigit(*p)) {
      return false;
    }
  }
  return true;
}

// Occasionally, BitPar produces labels containing additional numeric values
// between dashes.  For instance, "PN-DA-2-Dat.Sg.Masc" and "PN-OA-1-Acc.Sg".
// I don't whether the number belongs to the syntactic function, whether it's
// some kind of morphological tags, or whether it has some external meaning.
// For now, they are silently ignored.
void BitParLabelParser::RemoveMysteriousNumbers() const {
  std::vector<std::string>::iterator p;
  p = std::remove_if(parts_.begin(), parts_.end(), IsNumeric);
  parts_.erase(p, parts_.end());
}

}  // namespace de
}  // namespace tool
}  // namespace taco
