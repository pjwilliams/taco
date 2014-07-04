#include <boost/test/unit_test.hpp>

#include "taco/text-formats/constraint_tokeniser.h"

#include <sstream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE(TestConstraintTokeniser) {
  using namespace taco;
  using namespace taco::internal;

  // COLON
  {
    std::string s = ":";
    std::vector<ConstraintToken> tokens;
    ConstraintTokeniser end;
    for (ConstraintTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 1);
    BOOST_CHECK(tokens[0].type == ConstraintToken_COLON);
    BOOST_CHECK(tokens[0].value == ":");
    BOOST_CHECK(tokens[0].pos == 0);
  }

  // COMMA
  {
    std::string s = ",";
    std::vector<ConstraintToken> tokens;
    ConstraintTokeniser end;
    for (ConstraintTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 1);
    BOOST_CHECK(tokens[0].type == ConstraintToken_COMMA);
    BOOST_CHECK(tokens[0].value == ",");
    BOOST_CHECK(tokens[0].pos == 0);
  }

  // DIGITSEQ
  {
    std::string s = "1 0 123";
    std::vector<ConstraintToken> tokens;
    ConstraintTokeniser end;
    for (ConstraintTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 3);
    BOOST_CHECK(tokens[0].type == ConstraintToken_DIGITSEQ);
    BOOST_CHECK(tokens[0].value == "1");
    BOOST_CHECK(tokens[0].pos == 0);
    BOOST_CHECK(tokens[1].type == ConstraintToken_DIGITSEQ);
    BOOST_CHECK(tokens[1].value == "0");
    BOOST_CHECK(tokens[1].pos == 2);
    BOOST_CHECK(tokens[2].type == ConstraintToken_DIGITSEQ);
    BOOST_CHECK(tokens[2].value == "123");
    BOOST_CHECK(tokens[2].pos == 4);
  }

  // EQUALS
  {
    std::string s = "=";
    std::vector<ConstraintToken> tokens;
    ConstraintTokeniser end;
    for (ConstraintTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 1);
    BOOST_CHECK(tokens[0].type == ConstraintToken_EQUALS);
    BOOST_CHECK(tokens[0].value == "=");
    BOOST_CHECK(tokens[0].pos == 0);
  }

  // PROBABILITY
  {
    std::string s = "0.06 0.730 1.0 0.0";
    std::vector<ConstraintToken> tokens;
    ConstraintTokeniser end;
    for (ConstraintTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 4);
    BOOST_CHECK(tokens[0].type == ConstraintToken_PROBABILITY);
    BOOST_CHECK(tokens[0].value == "0.06");
    BOOST_CHECK(tokens[0].pos == 0);
    BOOST_CHECK(tokens[1].type == ConstraintToken_PROBABILITY);
    BOOST_CHECK(tokens[1].value == "0.730");
    BOOST_CHECK(tokens[1].pos == 5);
    BOOST_CHECK(tokens[2].type == ConstraintToken_PROBABILITY);
    BOOST_CHECK(tokens[2].value == "1.0");
    BOOST_CHECK(tokens[2].pos == 11);
    BOOST_CHECK(tokens[3].type == ConstraintToken_PROBABILITY);
    BOOST_CHECK(tokens[3].value == "0.0");
    BOOST_CHECK(tokens[3].pos == 15);
  }

  // LANGLE
  {
    std::string s = "<";
    std::vector<ConstraintToken> tokens;
    ConstraintTokeniser end;
    for (ConstraintTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 1);
    BOOST_CHECK(tokens[0].type == ConstraintToken_LANGLE);
    BOOST_CHECK(tokens[0].value == "<");
    BOOST_CHECK(tokens[0].pos == 0);
  }

  // LCURLY
  {
    std::string s = "{";
    std::vector<ConstraintToken> tokens;
    ConstraintTokeniser end;
    for (ConstraintTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 1);
    BOOST_CHECK(tokens[0].type == ConstraintToken_LCURLY);
    BOOST_CHECK(tokens[0].value == "{");
    BOOST_CHECK(tokens[0].pos == 0);
  }

  // RANGLE
  {
    std::string s = ">";
    std::vector<ConstraintToken> tokens;
    ConstraintTokeniser end;
    for (ConstraintTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 1);
    BOOST_CHECK(tokens[0].type == ConstraintToken_RANGLE);
    BOOST_CHECK(tokens[0].value == ">");
    BOOST_CHECK(tokens[0].pos == 0);
  }

  // RCURLY
  {
    std::string s = "}";
    std::vector<ConstraintToken> tokens;
    ConstraintTokeniser end;
    for (ConstraintTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 1);
    BOOST_CHECK(tokens[0].type == ConstraintToken_RCURLY);
    BOOST_CHECK(tokens[0].value == "}");
    BOOST_CHECK(tokens[0].pos == 0);
  }

  // STRING
  {
    std::string s = "\"foo\" \"bar\"\"baz\"";
    std::vector<ConstraintToken> tokens;
    ConstraintTokeniser end;
    for (ConstraintTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 3);
    BOOST_CHECK(tokens[0].type == ConstraintToken_STRING);
    BOOST_CHECK(tokens[0].value == "foo");
    BOOST_CHECK(tokens[0].pos == 0);
    BOOST_CHECK(tokens[1].type == ConstraintToken_STRING);
    BOOST_CHECK(tokens[1].value == "bar");
    BOOST_CHECK(tokens[1].pos == 6);
    BOOST_CHECK(tokens[2].type == ConstraintToken_STRING);
    BOOST_CHECK(tokens[2].value == "baz");
    BOOST_CHECK(tokens[2].pos == 11);
  }

  // VARNAME
  {
    std::string s = "X X1 X123";
    std::vector<ConstraintToken> tokens;
    ConstraintTokeniser end;
    for (ConstraintTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 3);
    BOOST_CHECK(tokens[0].type == ConstraintToken_VARNAME);
    BOOST_CHECK(tokens[0].value == "X");
    BOOST_CHECK(tokens[0].pos == 0);
    BOOST_CHECK(tokens[1].type == ConstraintToken_VARNAME);
    BOOST_CHECK(tokens[1].value == "X1");
    BOOST_CHECK(tokens[1].pos == 2);
    BOOST_CHECK(tokens[2].type == ConstraintToken_VARNAME);
    BOOST_CHECK(tokens[2].value == "X123");
    BOOST_CHECK(tokens[2].pos == 5);
  }

  // empty string
  {
    std::string s = "";
    std::vector<ConstraintToken> tokens;
    ConstraintTokeniser end;
    for (ConstraintTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 0);
  }

  // whitespace and escaped characters
  {
    std::ostringstream out;
    out << "  \t " << "<" << " " << "123" << "\"f\\\"oo\"" << " ";
    std::string s = out.str();

    std::vector<ConstraintToken> tokens;
    ConstraintTokeniser end;
    for (ConstraintTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }

    BOOST_CHECK(tokens.size() == 3);
    BOOST_CHECK(tokens[0].type == ConstraintToken_LANGLE);
    BOOST_CHECK(tokens[0].value == "<");
    BOOST_CHECK(tokens[0].pos == 4);
    BOOST_CHECK(tokens[1].type == ConstraintToken_DIGITSEQ);
    BOOST_CHECK(tokens[1].value == "123");
    BOOST_CHECK(tokens[1].pos == 6);
    BOOST_CHECK(tokens[2].type == ConstraintToken_STRING);
    BOOST_CHECK(tokens[2].value == "f\"oo");
    BOOST_CHECK(tokens[2].pos == 9);
  }
}
