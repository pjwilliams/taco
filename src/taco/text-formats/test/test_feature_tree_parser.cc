#include <boost/test/unit_test.hpp>

#include "taco/text-formats/feature_tree_parser.h"

#include "taco/feature_structure.h"
#include "taco/base/vocabulary.h"

#include <boost/assign/std/vector.hpp>

#include <string>

BOOST_AUTO_TEST_CASE(testFTTokeniser) {
  using namespace taco;
  using namespace taco::internal;

  {
    std::string s = "";
    std::vector<FTToken> tokens;
    FTTokeniser end;
    for (FTTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.empty());
  }

  {
    std::string s = " \t[\t    ";
    std::vector<FTToken> tokens;
    FTTokeniser end;
    for (FTTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 1);
    BOOST_CHECK(tokens[0].type == FTToken_LB);
    BOOST_CHECK(tokens[0].value == "[");
  }

  {
    std::string s = "[AGREEMENT:[CASE;NUMBER];POS]";
    std::vector<FTToken> tokens;
    FTTokeniser end;
    for (FTTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 11);
    BOOST_CHECK(tokens[0].type == FTToken_LB);
    BOOST_CHECK(tokens[0].value == "[");
    BOOST_CHECK(tokens[1].type == FTToken_WORD);
    BOOST_CHECK(tokens[1].value == "AGREEMENT");
    BOOST_CHECK(tokens[2].type == FTToken_COLON);
    BOOST_CHECK(tokens[2].value == ":");
    BOOST_CHECK(tokens[3].type == FTToken_LB);
    BOOST_CHECK(tokens[3].value == "[");
    BOOST_CHECK(tokens[4].type == FTToken_WORD);
    BOOST_CHECK(tokens[4].value == "CASE");
    BOOST_CHECK(tokens[5].type == FTToken_SEMICOLON);
    BOOST_CHECK(tokens[5].value == ";");
    BOOST_CHECK(tokens[6].type == FTToken_WORD);
    BOOST_CHECK(tokens[6].value == "NUMBER");
    BOOST_CHECK(tokens[7].type == FTToken_RB);
    BOOST_CHECK(tokens[7].value == "]");
    BOOST_CHECK(tokens[8].type == FTToken_SEMICOLON);
    BOOST_CHECK(tokens[8].value == ";");
    BOOST_CHECK(tokens[9].type == FTToken_WORD);
    BOOST_CHECK(tokens[9].value == "POS");
    BOOST_CHECK(tokens[10].type == FTToken_RB);
    BOOST_CHECK(tokens[10].value == "]");
  }

  {
    std::string s = "[A:[D;F];B:[C:[E];G]]";
    std::vector<FTToken> tokens;
    FTTokeniser end;
    for (FTTokeniser t(s); t != end; ++t) {
      tokens.push_back(*t);
    }
    BOOST_CHECK(tokens.size() == 21);
    BOOST_CHECK(tokens[0].type == FTToken_LB);
    BOOST_CHECK(tokens[0].value == "[");
    BOOST_CHECK(tokens[1].type == FTToken_WORD);
    BOOST_CHECK(tokens[1].value == "A");
    BOOST_CHECK(tokens[2].type == FTToken_COLON);
    BOOST_CHECK(tokens[2].value == ":");
    // ...
    BOOST_CHECK(tokens[18].type == FTToken_WORD);
    BOOST_CHECK(tokens[18].value == "G");
    BOOST_CHECK(tokens[19].type == FTToken_RB);
    BOOST_CHECK(tokens[19].value == "]");
    BOOST_CHECK(tokens[20].type == FTToken_RB);
    BOOST_CHECK(tokens[20].value == "]");
  }
}

BOOST_AUTO_TEST_CASE(testFeatureTreeParser) {
  using namespace taco;
  using namespace boost::assign;

  std::string s = "[A:[D;F];B:[C:[E];G]]";
  Vocabulary feature_set;
  FeatureTreeParser parser(feature_set);
  boost::shared_ptr<FeatureTree> ft = parser.parse(s);

  BOOST_CHECK(ft);

  // Check all features have been added to feature_set
  Feature A = feature_set.Lookup("A");
  BOOST_CHECK(A != Vocabulary::NullId());

  Feature B = feature_set.Lookup("B");
  BOOST_CHECK(B != Vocabulary::NullId());

  Feature C = feature_set.Lookup("C");
  BOOST_CHECK(C != Vocabulary::NullId());

  Feature D = feature_set.Lookup("D");
  BOOST_CHECK(D != Vocabulary::NullId());

  Feature E = feature_set.Lookup("E");
  BOOST_CHECK(E != Vocabulary::NullId());

  Feature F = feature_set.Lookup("F");
  BOOST_CHECK(F != Vocabulary::NullId());

  Feature G = feature_set.Lookup("G");
  BOOST_CHECK(G != Vocabulary::NullId());

  BOOST_CHECK(ft->children_.size() == 2);

  // Check child A
  {
    boost::shared_ptr<FeatureTree> child = ft->children_[A];
    BOOST_CHECK(child->children_.size() == 2);

    // Check grandchild D
    boost::shared_ptr<FeatureTree> grandchild = child->children_[D];
    BOOST_CHECK(grandchild.get());
    BOOST_CHECK(grandchild->children_.empty());

    // Check grandchild F
    //grandchild.reset(child->children_[F]);
    grandchild = child->children_[F];
    BOOST_CHECK(grandchild.get());
    BOOST_CHECK(grandchild->children_.empty());

    // Check Lookup of non-existent grandchild
    grandchild = child->children_[B];
    BOOST_CHECK(!grandchild.get());
  }

  // TODO Check child B
}
