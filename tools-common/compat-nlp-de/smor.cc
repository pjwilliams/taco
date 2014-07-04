#include "tools-common/compat-nlp-de/smor.h"

#include "taco/base/exception.h"

#include <cassert>
#include <sstream>

namespace taco {
namespace tool {
namespace de {
namespace smor {

std::ostream &operator<<(std::ostream &out, const StemElement &elem) {
  if (elem.is_feature) {
    out << "<" << elem.text << ">";
  } else {
    out << elem.text;
  }
  return out;
}

std::ostream &operator<<(std::ostream &out, const Morpheme &m) {
  out << m.text;
  return out;
}

std::ostream &operator<<(std::ostream &out, const Feature &f) {
  out << "<" << f.text << ">";
  return out;
}

std::ostream &operator<<(std::ostream &out, const WordClass &wc) {
  out << "<+" << wc.text << ">";
  return out;
}

std::ostream &operator<<(std::ostream &out, const Stem &stem) {
  for (Stem::const_iterator p(stem.begin()); p != stem.end(); ++p) {
    out << *p;
  }
  return out;
}

std::ostream &operator<<(std::ostream &out, const Inflection &infl) {
  for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
    out << *p;
  }
  return out;
}

std::ostream &operator<<(std::ostream &out, const Analysis &analysis) {
  out << analysis.stem << analysis.word_class << analysis.inflection;
  return out;
}

Tokeniser::Tokeniser()
  : value_(Token_EOF, "", -1, -1)
  , input_(0)
  , line_num_(0)
  , skip_analyses_(false) {
}

Tokeniser::Tokeniser(std::istream &input)
  : value_(Token_EOF, "", -1, -1)
  , input_(&input)
  , line_num_(0)
  , skip_analyses_(false) {
  ++(*this);
}

Tokeniser &Tokeniser::operator++() {
  if (!input_) {
    return *this;
  }
  while (buffer_.empty()) {
    std::string line;
    std::getline(*input_, line);
    if (!(*input_)) {
      value_ = Token(Token_EOF, "", -1, -1);
      input_ = 0;
      return *this;
    }
    ++line_num_;
    TokeniseLine(line);
  }
  assert(!buffer_.empty());
  value_ = buffer_.front();
  buffer_.pop();
  return *this;
}

Tokeniser Tokeniser::operator++(int) {
  Tokeniser tmp(*this);
  ++*this;
  return tmp;
}

void Tokeniser::TokeniseLine(const std::string &line) {
  assert(buffer_.empty());

  // TODO Check for surprises: the very first line should always contain
  // a word, we shouldn't get two consecutive word lines...
  if (line.size() > 2 && line[0] == '>' && line[1] == ' ') {
    std::string word = line.substr(2, line.size()-2);
    // If the word contains a '<' or a '>' then don't even try to tokenise
    // the analyses...
    if (word.find_first_of("<>") != std::string::npos) {
      skip_analyses_ = true;
      return;
    }
    skip_analyses_ = false;
    buffer_.push(Token(Token_RANGLE, ">", line_num_, 1));
    buffer_.push(Token(Token_WORD, word, line_num_, 3));
    buffer_.push(Token(Token_NEWLINE, "\n", line_num_, line.size()+1));
    return;
  }

  if (skip_analyses_) {
    return;
  }

  if (line.find("no result for ") == 0) {
    buffer_.push(Token(Token_NORESULT, line, line_num_, 1));
    buffer_.push(Token(Token_NEWLINE, "\n", line_num_, line.size()+1));
    return;
  }

  bool inside_brackets = false;
  size_t word_start = 0;
  std::string word;
  for (size_t i = 0; i < line.size(); ++i) {
    if (line[i] == '<') {
      if (inside_brackets) {
        std::ostringstream msg;
        msg << "Unexpected character '<' at line "
            << line_num_ << ", col " << i+1;
        throw Exception(msg.str());
      }
      if (!word.empty()) {
        buffer_.push(Token(Token_WORD, word, line_num_, word_start+1));
      }
      if (i+1 < line.size() && line[i+1] == '+') {
        buffer_.push(Token(Token_LANGLE_PLUS, "<+", line_num_, i+1));
        ++i;
      } else {
        buffer_.push(Token(Token_LANGLE, "<", line_num_, i+1));
      }
      inside_brackets = true;
      word_start = i+1;
      word = "";
    } else if (line[i] == '>') {
      if (!inside_brackets) {
        std::ostringstream msg;
        msg << "Unexpected character '>' at line "
            << line_num_ << ", col " << i+1;
        throw Exception(msg.str());
      }
      buffer_.push(Token(Token_WORD, word, line_num_, word_start+1));
      buffer_.push(Token(Token_RANGLE, ">", line_num_, i+1));
      inside_brackets = false;
      word_start = i+1;
      word = "";
    } else {
      word += line[i];
    }
  }
  if (!word.empty()) {
    if (inside_brackets) {
      std::ostringstream msg;
      msg << "Feature decoration missing closing '>' at line "
          << line_num_;
      throw Exception(msg.str());
    }
    buffer_.push(Token(Token_WORD, word, line_num_, word_start+1));
  }
  buffer_.push(Token(Token_NEWLINE, "\n", line_num_, line.size()+1));
}

bool operator==(const Tokeniser &lhs, const Tokeniser &rhs) {
  return (lhs.input_ == rhs.input_) ||
         (lhs.value_.type == Token_EOF && rhs.value_.type == Token_EOF);
}

bool operator!=(const Tokeniser &lhs, const Tokeniser &rhs) {
  return !(lhs == rhs);
}

Parser::Parser()
    : tokeniser_()
    , lookahead_(Token(Token_EOF, "", -1, -1))
    , done_(true) {
}

Parser::Parser(std::istream &input)
    : tokeniser_(input)
    , lookahead_(*tokeniser_)
    , done_(false) {
  ++*this;
}

Parser &Parser::operator++() {
  if (done_) {
    return *this;
  }

  if (lookahead_.type == Token_EOF) {
    Match(Token_EOF);
    done_ = true;
    return *this;
  }
  Match(Token_RANGLE);
  value_.first = Match(Token_WORD);
  Match(Token_NEWLINE);
  value_.second.clear();
  NT_Result(value_.second);
  return *this;
}

Parser Parser::operator++(int) {
  Parser tmp(*this);
  ++*this;
  return tmp;
}

std::string Parser::Match(TokenType expected_type) {
  if (lookahead_.type != expected_type) {
    std::ostringstream msg;
    msg << "Unexpected token: expected `" << expected_type
        << "', got `" << lookahead_.type << "'";
    throw Exception(msg.str());
  }
  std::string value = lookahead_.value;
  lookahead_ = *(++tokeniser_);
  return value;
}

void Parser::NT_Result(Analyses &analyses) {
  if (lookahead_.type == Token_NORESULT) {
    Match(Token_NORESULT);
    Match(Token_NEWLINE);
    return;
  }

  while (lookahead_.type == Token_WORD || lookahead_.type == Token_LANGLE) {
    NT_Analysis(analyses);
    Match(Token_NEWLINE);
  }
}

void Parser::NT_Analysis(Analyses &analyses) {
  analyses.resize(analyses.size()+1);
  Analysis &analysis = analyses.back();
  analysis.stem.push_back(NT_StemElement());
  while (lookahead_.type == Token_WORD || lookahead_.type == Token_LANGLE) {
    analysis.stem.push_back(NT_StemElement());
  }
  Match(Token_LANGLE_PLUS);
  analysis.word_class = Match(Token_WORD);
  Match(Token_RANGLE);
  while (lookahead_.type == Token_LANGLE) {
    Match(Token_LANGLE);
    analysis.inflection.push_back(Match(Token_WORD));
    Match(Token_RANGLE);
  }
}

StemElement Parser::NT_StemElement() {
  if (lookahead_.type == Token_LANGLE) {
    Match(Token_LANGLE);
    StemElement element(Match(Token_WORD), true);
    Match(Token_RANGLE);
    return element;
  }
  return StemElement(Match(Token_WORD), false);
}

bool operator==(const Parser &lhs, const Parser &rhs) {
  // Note that since we're looking ahead by one token, it's not enough to test
  // that the tokenisers have both reached the end.  We need an additional
  // 'done' flag that's set after the EOF token is *matched*, otherwise a loop
  // like:
  //
  //      for (Parser p(input); p != end; ++p)
  //          foo(*p);
  //
  // would prematurely terminate, missing the final word-analyses pair.
  return (lhs.tokeniser_ == rhs.tokeniser_) && (lhs.done_ == rhs.done_);
}

bool operator!=(const Parser &lhs, const Parser &rhs) {
  return !(lhs == rhs);
}

}  // namespace smor
}  // namespace de
}  // namespace tool
}  // namespace taco
