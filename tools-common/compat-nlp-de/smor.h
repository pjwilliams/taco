#ifndef TACO_TOOLS_COMMON_COMPAT_NLP_DE_SMOR_H_
#define TACO_TOOLS_COMMON_COMPAT_NLP_DE_SMOR_H_

#include <istream>
#include <ostream>
#include <queue>
#include <string>
#include <vector>

namespace taco {
namespace tool {
namespace de {
namespace smor {

struct StemElement {
 public:
  StemElement(const std::string &text_, bool is_feature_)
      : text(text_)
      , is_feature(is_feature_) {}

  std::string text;
  bool is_feature;
};

struct Morpheme : public StemElement {
 public:
  Morpheme(const std::string &text_ = "") : StemElement(text_, false) {}
};

struct Feature : public StemElement {
 public:
  Feature(const std::string &text_ = "") : StemElement(text_, true) {}

  bool IsAcc() const { return text == "Akk" || text == "Acc"; }
  bool IsAttr() const { return text == "attr" || text == "Attr"; }
  bool IsDat() const { return text == "Dat"; }
  bool IsGen() const { return text == "Gen"; }
  bool IsNom() const { return text == "Nom"; }
  bool IsPers() const { return text == "pers" || text == "Pers"; }
  bool IsPrfl() const { return text == "prfl" || text == "Prfl"; }
  bool IsPro() const { return text == "pro" || text == "Pro"; }
  bool IsRefl() const { return text == "refl" || text == "Refl"; }
  bool IsRez() const { return text == "rez" || text == "Rec"; }
  bool IsSt() const { return text == "St"; }
  bool IsStMix() const { return text == "St/Mix"; }
  bool IsSubst() const { return text == "subst" || text == "Subst"; }
  bool IsWk() const { return text == "Sw" || text == "Wk"; }
  bool IsWkMix() const { return text == "Sw/Mix"; }
};

struct WordClass {
 public:
  WordClass(const std::string &text_ = "") : text(text_) {}
  std::string text;
};

std::ostream &operator<<(std::ostream &, const StemElement &);
std::ostream &operator<<(std::ostream &, const Morpheme &);
std::ostream &operator<<(std::ostream &, const Feature &);
std::ostream &operator<<(std::ostream &, const WordClass &);

typedef std::vector<StemElement> Stem;
typedef std::vector<Feature> Inflection;

std::ostream &operator<<(std::ostream &, const Stem &);
std::ostream &operator<<(std::ostream &, const Inflection &);

struct Analysis {
 public:
  Stem stem;
  WordClass word_class;
  Inflection inflection;
};

typedef std::vector<Analysis> Analyses;
typedef std::pair<std::string, Analyses> WordAnalysesPair;

std::ostream &operator<<(std::ostream &, const Analysis &);

enum TokenType {
  Token_EOF,
  Token_LANGLE,
  Token_LANGLE_PLUS,
  Token_NEWLINE,
  Token_NORESULT,
  Token_RANGLE,
  Token_WORD
};

struct Token {
 public:
  Token(TokenType type_, const std::string &value_,
        size_t line_num_, size_t char_num_)
      : type(type_)
      , value(value_)
      , line_num(line_num_)
      , char_num(char_num_) {}

  TokenType type;
  std::string value;
  size_t line_num;
  size_t char_num;
};

////////////////////////////////////////////////////////////////////////////
//
// Class for tokenising SMOR output.  Used by the parser.  It implements
// the input iterator interface, yielding a stream of Token objects.
//
// Note: SMOR doesn't escape the '<' and '>' characters in morphemes,
// which makes tokenising difficult since those characters are used to
// indicate feature decorations.  The current strategy is to check the
// analysed word for those characters and if it contains one then to
// silently skip over the word and its analyses.
//
////////////////////////////////////////////////////////////////////////////

// TODO Set traits
class Tokeniser {
 public:
  Tokeniser();
  Tokeniser(std::istream &);

  const Token &operator*() const { return value_; }
  const Token *operator->() const { return &value_; }

  Tokeniser &operator++();
  Tokeniser operator++(int);

  friend bool operator==(const Tokeniser &, const Tokeniser &);
  friend bool operator!=(const Tokeniser &, const Tokeniser &);

 private:
  void TokeniseLine(const std::string &line);

  Token value_;
  std::istream *input_;
  std::queue<Token> buffer_;
  size_t line_num_;
  bool skip_analyses_;
};

////////////////////////////////////////////////////////////////////////////
//
// Class for parsing SMOR output.  It accepts the following grammar:
//
//                   S -> OutputList
//
//          OutputList -> RANGLE WORD NEWLINE Result OutputList
//                      | EOF
//
//              Result -> Analysis NEWLINE AnalysisList
//                      | NORESULT NEWLINE
//
//            Analysis -> Stem LANGLE_PLUS WordClass RANGLE InflFeatures
//
//        AnalysisList -> Analysis NEWLINE AnalysisList
//                      | E
//
//                Stem -> StemElement StemElementList
//
//         StemElement -> Morpheme
//                      | LANGLE Feature RANGLE
//
//     StemElementList -> StemElement StemElementList
//                      | E
//
//        InflFeatures -> LANGLE Feature RANGLE InflFeatures
//                      | E
//
//            Morpheme -> WORD
//
//             Feature -> WORD
//
//           WordClass -> WORD
//
// and produces a stream of word-analyses pairs.  It implements the input
// iterator interface.
//
// Note that the tokeniser silently drops analyses where the word contains
// a left or right angle bracket character ('<' or '>') so these will be
// absent from the word-analyses pairs.
//
// The implementation is a predictive parser with one token of lookahead.
//
////////////////////////////////////////////////////////////////////////////

// TODO Set traits
class Parser {
 public:
  Parser();
  Parser(std::istream &);

  const WordAnalysesPair &operator*() const { return value_; }
  const WordAnalysesPair *operator->() const { return &value_; }

  Parser &operator++();
  Parser operator++(int);

  friend bool operator==(const Parser &, const Parser &);
  friend bool operator!=(const Parser &, const Parser &);

 private:
  std::string Match(TokenType);

  void NT_Result(Analyses &);
  void NT_Analysis(Analyses &);
  StemElement NT_StemElement();

  WordAnalysesPair value_;
  Tokeniser tokeniser_;
  Token lookahead_;
  bool done_;
};

}  // namespace smor
}  // namespace de
}  // namespace tool
}  // namespace taco

#endif
