#include <boost/test/unit_test.hpp>

#include "taco/text-formats/feature_structure_tokeniser.h"

#include "taco/feature_structure.h"
#include "taco/base/vocabulary.h"

#include <boost/assign/std/vector.hpp>

#include <string>

BOOST_AUTO_TEST_CASE(TestFSTokeniser) {
  using namespace taco;
  using namespace taco::internal;

  {
    std::string s = "";
    std::vector<FSToken> tokens;
    FSTokeniser end;
    for (FSTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.empty());
  }

  {
    std::string s = " \t[\t    ";
    std::vector<FSToken> tokens;
    FSTokeniser end;
    for (FSTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 1);
    BOOST_CHECK(tokens[0].type == FSToken_LB);
    BOOST_CHECK(tokens[0].value == "[");
  }

  {
    std::string s = "[POS:NN;CASE:gen]";
    std::vector<FSToken> tokens;
    FSTokeniser end;
    for (FSTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 8);
    BOOST_CHECK(tokens[0].type == FSToken_LB);
    BOOST_CHECK(tokens[0].value == "[");
    BOOST_CHECK(tokens[1].type == FSToken_WORD);
    BOOST_CHECK(tokens[1].value == "POS");
    BOOST_CHECK(tokens[2].type == FSToken_COLON);
    BOOST_CHECK(tokens[2].value == ":");
    BOOST_CHECK(tokens[3].type == FSToken_WORD);
    BOOST_CHECK(tokens[3].value == "NN");
    BOOST_CHECK(tokens[4].type == FSToken_WORD);
    BOOST_CHECK(tokens[4].value == "CASE");
    BOOST_CHECK(tokens[5].type == FSToken_COLON);
    BOOST_CHECK(tokens[5].value == ":");
    BOOST_CHECK(tokens[6].type == FSToken_WORD);
    BOOST_CHECK(tokens[6].value == "gen");
    BOOST_CHECK(tokens[7].type == FSToken_RB);
    BOOST_CHECK(tokens[7].value == "]");
  }

  {
    std::string s = "[A:[D:x;F:z];B:[C:[E:y];G:w]]";
    std::vector<FSToken> tokens;
    FSTokeniser end;
    for (FSTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 26);
    BOOST_CHECK(tokens[0].type == FSToken_LB);
    BOOST_CHECK(tokens[0].value == "[");
    BOOST_CHECK(tokens[1].type == FSToken_WORD);
    BOOST_CHECK(tokens[1].value == "A");
    BOOST_CHECK(tokens[2].type == FSToken_COLON);
    BOOST_CHECK(tokens[2].value == ":");
    BOOST_CHECK(tokens[3].type == FSToken_LB);
    BOOST_CHECK(tokens[3].value == "[");
    BOOST_CHECK(tokens[4].type == FSToken_WORD);
    BOOST_CHECK(tokens[4].value == "D");
    BOOST_CHECK(tokens[5].type == FSToken_COLON);
    BOOST_CHECK(tokens[5].value == ":");
    BOOST_CHECK(tokens[6].type == FSToken_WORD);
    BOOST_CHECK(tokens[6].value == "x");
    BOOST_CHECK(tokens[7].type == FSToken_WORD);
    BOOST_CHECK(tokens[7].value == "F");
    BOOST_CHECK(tokens[8].type == FSToken_COLON);
    BOOST_CHECK(tokens[8].value == ":");
    BOOST_CHECK(tokens[9].type == FSToken_WORD);
    BOOST_CHECK(tokens[9].value == "z");
    BOOST_CHECK(tokens[10].type == FSToken_RB);
    BOOST_CHECK(tokens[10].value == "]");
    BOOST_CHECK(tokens[11].type == FSToken_WORD);
    BOOST_CHECK(tokens[11].value == "B");
    BOOST_CHECK(tokens[12].type == FSToken_COLON);
    BOOST_CHECK(tokens[12].value == ":");
    BOOST_CHECK(tokens[13].type == FSToken_LB);
    BOOST_CHECK(tokens[13].value == "[");
    BOOST_CHECK(tokens[14].type == FSToken_WORD);
    BOOST_CHECK(tokens[14].value == "C");
    BOOST_CHECK(tokens[15].type == FSToken_COLON);
    BOOST_CHECK(tokens[15].value == ":");
    // ...
    BOOST_CHECK(tokens[23].type == FSToken_WORD);
    BOOST_CHECK(tokens[23].value == "w");
    BOOST_CHECK(tokens[24].type == FSToken_RB);
    BOOST_CHECK(tokens[24].value == "]");
    BOOST_CHECK(tokens[25].type == FSToken_RB);
    BOOST_CHECK(tokens[25].value == "]");
  }
}
