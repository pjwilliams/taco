#include "taco/text-formats/feature_tree_parser.h"

#include <cctype>

namespace taco {
namespace internal {

FTToken::FTToken(FTTokenType t, const std::string &v, size_t p)
  : type(t)
  , value(v)
  , pos(p) {
}

FTTokeniser::FTTokeniser()
  : m_value(FTToken_EOS, "", -1)
  , m_input(0) {
}

FTTokeniser::FTTokeniser(const std::string &s)
  : m_value(FTToken_EOS, "", -1)
  , m_input(&s)
  , m_iter(s.begin())
  , m_end(s.end())
  , m_pos(0) {
  ++(*this);
}

FTTokeniser &
FTTokeniser::operator++() {
  while (m_iter != m_end && (*m_iter == ' ' || *m_iter == '\t')) {
    ++m_iter;
    ++m_pos;
  }

  if (m_iter == m_end) {
    m_value = FTToken(FTToken_EOS, "", m_pos);
    return *this;
  }

  if (*m_iter == '[') {
    m_value = FTToken(FTToken_LB, "[", m_pos);
    ++m_iter;
    ++m_pos;
  } else if (*m_iter == ']') {
    m_value = FTToken(FTToken_RB, "]", m_pos);
    ++m_iter;
    ++m_pos;
  } else if (*m_iter == ':') {
    m_value = FTToken(FTToken_COLON, ":", m_pos);
    ++m_iter;
    ++m_pos;
  } else if (*m_iter == ';') {
    m_value = FTToken(FTToken_SEMICOLON, ";", m_pos);
    ++m_iter;
    ++m_pos;
  } else {
    std::string s;
    size_t start = m_pos;
    while (true) {
      s += *m_iter++;
      ++m_pos;
      if (m_iter == m_end || *m_iter == ' ' || *m_iter == '\t') {
        break;
      }
      if (*m_iter == '[' || *m_iter == ']' || *m_iter == ':' ||
          *m_iter == ';') {
        break;
      }
    }
    m_value = FTToken(FTToken_WORD, s, start);
  }

  return *this;
}

FTTokeniser FTTokeniser::operator++(int) {
  FTTokeniser tmp(*this);
  ++*this;
  return tmp;
}

bool operator==(const FTTokeniser &lhs, const FTTokeniser &rhs) {
  // TODO Check me.
  // TODO Write a unit test.
  if (lhs.m_input && (lhs.m_input == rhs.m_input)) {
    return true;
  }
  return (lhs.m_value.type == FTToken_EOS && rhs.m_value.type == FTToken_EOS);
}

bool operator!=(const FTTokeniser &lhs, const FTTokeniser &rhs) {
  return !(lhs == rhs);
}

}  // namespace internal

boost::shared_ptr<FeatureTree> FeatureTreeParser::parse(
    const std::string &s) const {
  m_tokeniser = internal::FTTokeniser(s);
  m_lookahead = *m_tokeniser;
  return ntFT();
}

std::string FeatureTreeParser::match(internal::FTTokenType expectedType) const {
  if (m_lookahead.type != expectedType) {
    std::ostringstream msg;
    msg << "Unexpected token: expected `" << expectedType
        << "', got `" << m_lookahead.type << "'";
    throw Exception(msg.str());
  }
  std::string val = m_lookahead.value;
  m_lookahead = *++m_tokeniser;
  return val;
}

boost::shared_ptr<FeatureTree> FeatureTreeParser::ntFT() const {
  boost::shared_ptr<FeatureTree> tree(new FeatureTree());
  match(internal::FTToken_LB);
  ntEntryList(*tree);
  match(internal::FTToken_RB);
  return tree;
}

void FeatureTreeParser::ntEntryList(FeatureTree &tree) const {
  if (m_lookahead.type != internal::FTToken_WORD) {
    return;
  }
  ntEntry(tree);
  ntEntryListTail(tree);
}

void FeatureTreeParser::ntEntryListTail(FeatureTree &tree) const {
  if (m_lookahead.type != internal::FTToken_SEMICOLON) {
    return;
  }
  match(internal::FTToken_SEMICOLON);
  ntEntryList(tree);
}

void FeatureTreeParser::ntEntry(FeatureTree &tree) const {
  Feature feature = m_featureSet.Insert(match(internal::FTToken_WORD));
  tree.children_[feature] = ntEntryTail();
}

boost::shared_ptr<FeatureTree> FeatureTreeParser::ntEntryTail() const {
  if (m_lookahead.type != internal::FTToken_COLON) {
    return boost::shared_ptr<FeatureTree>(new FeatureTree());
  }
  match(internal::FTToken_COLON);
  return ntFT();
}

}  // namespace taco
