#include <boost/test/unit_test.hpp>

#include "taco/constraint_evaluator.h"

#include "taco/constraint.h"
#include "taco/constraint_set.h"
#include "taco/feature_structure.h"
#include "taco/interpretation.h"
#include "taco/option_table.h"
#include "taco/text-formats/constraint_set_parser.h"
#include "taco/text-formats/feature_structure_parser.h"
#include "taco/base/vocabulary.h"

#include <boost/assign/std/set.hpp>
#include <boost/assign/std/vector.hpp>

#include <string>
#include <utility>
#include <vector>

namespace {

void ParseAndAddOptions(const std::vector<std::string> & fs_strings,
                        taco::FeatureStructureParser &fs_parser,
                        size_t index,
                        taco::OptionTable &option_table) {
  using namespace taco;

  if (fs_strings.empty()) {
    option_table.AddEmptyColumn(index);
  } else {
    OptionColumn col;
    for (std::vector<std::string>::const_iterator p(fs_strings.begin());
         p != fs_strings.end(); ++p) {
      col.Add(fs_parser.Parse(*p));
    }
    option_table.AddColumn(index, col);
  }
}

}  // end anonymous namespace

BOOST_AUTO_TEST_CASE(TestConstraintEvaluator1) {
  using namespace taco;
  using namespace boost::assign;

  Vocabulary feature_set;
  Vocabulary value_set;

  FeatureStructureParser fs_parser(feature_set, value_set);

  OptionTable option_table;

  // Insert entries for element 11.
  {
    std::vector<std::string> options;
    options += "[A:x;B:z]";
    size_t index = 11;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  // Insert entries for element 3.
  {
    std::vector<std::string> options;
    options += "[A:x;B:y]";
    size_t index = 3;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  // Insert entries for element 8.
  {
    std::vector<std::string> options;
    options += "[A:x]";
    size_t index = 8;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  BOOST_CHECK(option_table.Size() == 3);

  // Test first constraint set.
  {
    std::string s = std::string("<3>=<8>");

    ConstraintSetParser parser(feature_set, value_set);
    boost::shared_ptr<ConstraintSet> constraintSet = parser.Parse(s);

    ConstraintEvaluator evaluator;
    std::vector<Interpretation> interpretations;
    BOOST_CHECK(evaluator.Eval(option_table, *constraintSet,
                               interpretations));
    BOOST_CHECK(interpretations.size() == 1);
  }

  // Test second constraint set.
  {
    std::string s = std::string("<3>=<8> <8>=<11>");

    ConstraintSetParser parser(feature_set, value_set);
    boost::shared_ptr<ConstraintSet> constraintSet = parser.Parse(s);

    ConstraintEvaluator evaluator;
    std::vector<Interpretation> interpretations;
    BOOST_CHECK(!evaluator.Eval(option_table, *constraintSet,
                                interpretations));
    BOOST_CHECK(interpretations.empty());
  }
}

BOOST_AUTO_TEST_CASE(TestConstraintEvaluator2) {
  using namespace taco;
  using namespace boost::assign;

  Vocabulary feature_set;
  Vocabulary value_set;

  FeatureStructureParser fs_parser(feature_set, value_set);

  OptionTable option_table;

  // Insert entries for element 0, the rule LHS.
  {
    std::vector<std::string> options;
    options += "[]";
    size_t index = 0;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  // Insert entries for element 1, "die."
  {
    std::vector<std::string> options;
    options += "[AGR:[CASE:nom;DECL:weak;NUMBER:pl];POS:ART]",
               "[AGR:[CASE:acc;DECL:weak;NUMBER:pl];POS:ART]",
               "[AGR:[CASE:nom;DECL:weak;GENDER:f;NUMBER:sg];POS:ART]",
               "[AGR:[CASE:acc;DECL:weak;GENDER:f;NUMBER:sg];POS:ART]";
    size_t index = 1;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  // Insert entries for element 3, "kleine."
  {
    std::vector<std::string> options;
    options += "[AGR:[CASE:nom;DECL:strong;NUMBER:pl];POS:ADJA]",
               "[AGR:[CASE:acc;DECL:strong;NUMBER:pl];POS:ADJA]",
               "[AGR:[CASE:nom;GENDER:f;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:acc;GENDER:f;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:nom;DECL:weak;GENDER:n;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:acc;DECL:weak;GENDER:n;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:nom;DECL:weak;GENDER:m;NUMBER:sg];POS:ADJA]";
    size_t index = 3;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  // Insert entries for element 4, "Katze."
  {
    std::vector<std::string> options;
    options += "[AGR:[CASE:nom;GENDER:f;NUMBER:sg];POS:NN]",
               "[AGR:[CASE:acc;GENDER:f;NUMBER:sg];POS:NN]",
               "[AGR:[CASE:dat;GENDER:f;NUMBER:sg];POS:NN]",
               "[AGR:[CASE:gen;GENDER:f;NUMBER:sg];POS:NN]";
    size_t index = 4;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  BOOST_CHECK(option_table.Size() == 4);

  std::string s = std::string("<0\"AGR\">=<1\"AGR\">")
                  + std::string("<0\"AGR\">=<3\"AGR\">")
                  + std::string("<0\"AGR\">=<4\"AGR\">");

  ConstraintSetParser parser(feature_set, value_set);
  boost::shared_ptr<ConstraintSet> constraintSet = parser.Parse(s);

  ConstraintEvaluator evaluator;
  std::vector<Interpretation> interpretations;
  BOOST_CHECK(evaluator.Eval(option_table, *constraintSet, interpretations));
  BOOST_CHECK(interpretations.size() == 2);
}

BOOST_AUTO_TEST_CASE(TestConstraintEvaluator3) {
  using namespace taco;
  using namespace boost::assign;

  Vocabulary feature_set;
  Vocabulary value_set;

  FeatureStructureParser fs_parser(feature_set, value_set);

  OptionTable option_table;

  // Insert entries for element 3, "der."
  {
    std::vector<std::string> options;
    options += "[AGR:[CASE:nom;DECL:weak;GENDER:m;NUMBER:sg];POS:ART]",
               "[AGR:[CASE:gen;DECL:weak;NUMBER:pl];POS:ART]",
               "[AGR:[CASE:gen;DECL:weak;GENDER:f;NUMBER:sg];POS:ART]",
               "[AGR:[CASE:dat;DECL:weak;GENDER:f;NUMBER:sg];POS:ART]";
    size_t index = 3;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  // Insert entries for element 4, "grossen."
  {
    std::vector<std::string> options;
    options += "[AGR:[];POS:NN]",
               "[AGR:[CASE:nom;DECL:weak;NUMBER:pl];POS:ADJA]",
               "[AGR:[CASE:nom;DECL:mixed;NUMBER:pl];POS:ADJA]",
               "[AGR:[CASE:acc;DECL:weak;NUMBER:pl];POS:ADJA]",
               "[AGR:[CASE:acc;DECL:mixed;NUMBER:pl];POS:ADJA]",
               "[AGR:[CASE:acc;GENDER:m;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:gen;DECL:weak;NUMBER:pl];POS:ADJA]",
               "[AGR:[CASE:gen;DECL:weak;GENDER:f;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:gen;DECL:mixed;NUMBER:pl];POS:ADJA]",
               "[AGR:[CASE:gen;DECL:mixed;GENDER:f;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:gen;GENDER:n;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:gen;GENDER:m;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:dat;DECL:weak;GENDER:f;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:dat;DECL:weak;GENDER:n;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:dat;DECL:weak;GENDER:m;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:dat;DECL:mixed;GENDER:f;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:dat;DECL:mixed;GENDER:n;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:dat;DECL:mixed;GENDER:m;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:dat;NUMBER:pl];POS:ADJA]";
    size_t index = 4;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  // Insert entries for element 5, "Mehrheit."
  {
    std::vector<std::string> options;
    options += "[AGR:[CASE:nom;GENDER:f;NUMBER:sg];POS:NN]",
               "[AGR:[CASE:acc;GENDER:f;NUMBER:sg];POS:NN]",
               "[AGR:[CASE:gen;GENDER:f;NUMBER:sg];POS:NN]",
               "[AGR:[CASE:dat;GENDER:f;NUMBER:sg];POS:NN]";
    size_t index = 5;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  BOOST_CHECK(option_table.Size() == 3);

  std::string s = std::string("<3\"AGR\">=<4\"AGR\">")
                  + std::string("<3\"AGR\">=<5\"AGR\">")
                  + std::string("<3\"POS\">=\"ART\"")
                  + std::string("<4\"POS\">=\"ADJA\"")
                  + std::string("<5\"POS\">=\"NN\"")
                  + std::string("<3\"AGR\"\"CASE\">={")
                  +     std::string("\"nom\":0.058,")
                  +     std::string("\"acc\":0.006,")
                  +     std::string("\"gen\":0.730,")
                  +     std::string("\"dat\":0.206")
                  + std::string("}");

  ConstraintSetParser parser(feature_set, value_set);
  boost::shared_ptr<ConstraintSet> cs = parser.Parse(s);

  ConstraintEvaluator evaluator;
  std::vector<Interpretation> interpretations;
  BOOST_CHECK(evaluator.Eval(option_table, *cs, interpretations));
  BOOST_CHECK(interpretations.size() == 2);

  float p0 = interpretations[0].probability();
  float p1 = interpretations[1].probability();
  BOOST_CHECK((p0 == 0.730f && p1 == 0.206f) ||
              (p1 == 0.730f && p0 == 0.206f));
}

BOOST_AUTO_TEST_CASE(TestConstraintEvaluator4) {
  using namespace taco;
  using namespace boost::assign;

  Vocabulary feature_set;
  Vocabulary value_set;

  FeatureStructureParser fs_parser(feature_set, value_set);

  OptionTable option_table;

  // Insert entries for element 3.
  {
    std::vector<std::string> options;
    options += "[AGR:[CASE:nom;DECL:weak;GENDER:m;NUMBER:sg];POS:ART]",
               "[AGR:[CASE:nom;DECL:weak;NUMBER:pl];POS:ART]",
               "[AGR:[DECL:weak;GENDER:f;NUMBER:sg];POS:ART]";
    size_t index = 3;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  // Insert entries for element 2.
  {
    size_t index = 2;
    option_table.AddWildcardColumn(index);
  }

  // Insert entries for element 1.
  {
    std::vector<std::string> options;
    options += "[AGR:[GENDER:f;NUMBER:sg];POS:NN]";
    size_t index = 1;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  BOOST_CHECK(option_table.Size() == 3);

  std::string s = std::string("<1\"AGR\">=<2\"AGR\">")
                  + std::string("<1\"AGR\">=<3\"AGR\">")
                  + std::string("<3\"POS\">=\"ART\"")
                  + std::string("<2\"POS\">=\"ADJA\"")
                  + std::string("<1\"POS\">=\"NN\"")
                  + std::string("<1\"AGR\"\"CASE\">={")
                  +     std::string("\"nom\":0.058,")
                  +     std::string("\"acc\":0.006,")
                  +     std::string("\"gen\":0.730,")
                  +     std::string("\"dat\":0.206")
                  + std::string("}");

  ConstraintSetParser parser(feature_set, value_set);
  boost::shared_ptr<ConstraintSet> cs = parser.Parse(s);

  ConstraintEvaluator evaluator;
  std::vector<Interpretation> interpretations;
  BOOST_CHECK(evaluator.Eval(option_table, *cs, interpretations));
  BOOST_CHECK(interpretations.size() == 1);

  float p0 = interpretations[0].probability();
  BOOST_CHECK((p0 == 0.730f));
}

// Try option table column containing empty feature structure.
BOOST_AUTO_TEST_CASE(TestConstraintEvaluator5) {
  using namespace taco;
  using namespace boost::assign;

  Vocabulary feature_set;
  Vocabulary value_set;

  FeatureStructureParser fs_parser(feature_set, value_set);

  OptionTable option_table;

  // Insert entries for element 0, the rule LHS.
  {
    std::vector<std::string> options;
    options += "[]";
    size_t index = 0;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  // Insert entries for element 1, "die."
  {
    std::vector<std::string> options;
    options += "[AGR:[CASE:nom;DECL:weak;NUMBER:pl];POS:ART]",
               "[AGR:[CASE:acc;DECL:weak;NUMBER:pl];POS:ART]",
               "[AGR:[CASE:nom;DECL:weak;GENDER:f;NUMBER:sg];POS:ART]",
               "[AGR:[CASE:acc;DECL:weak;GENDER:f;NUMBER:sg];POS:ART]";
    size_t index = 1;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  // Insert entries for element 3, "kleine."
  {
    std::vector<std::string> options;
    options += "[AGR:[CASE:nom;DECL:strong;NUMBER:pl];POS:ADJA]",
               "[AGR:[CASE:acc;DECL:strong;NUMBER:pl];POS:ADJA]",
               "[AGR:[CASE:nom;GENDER:f;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:acc;GENDER:f;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:nom;DECL:weak;GENDER:n;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:acc;DECL:weak;GENDER:n;NUMBER:sg];POS:ADJA]",
               "[AGR:[CASE:nom;DECL:weak;GENDER:m;NUMBER:sg];POS:ADJA]";
    size_t index = 3;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  // Insert entries for element 4, "Foo."
  {
    std::vector<std::string> options;
    options += "[]";
    size_t index = 4;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  BOOST_CHECK(option_table.Size() == 4);

  std::string s = std::string("<0\"AGR\">=<1\"AGR\">")
                + std::string("<0\"AGR\">=<3\"AGR\">")
                + std::string("<0\"AGR\">=<4\"AGR\">");

  ConstraintSetParser parser(feature_set, value_set);
  boost::shared_ptr<ConstraintSet> cs = parser.Parse(s);

  ConstraintEvaluator evaluator;
  std::vector<Interpretation> interpretations;
  BOOST_CHECK(evaluator.Eval(option_table, *cs, interpretations));
  BOOST_CHECK(interpretations.size() == 2);
}

// Try empty OptionTable column.
BOOST_AUTO_TEST_CASE(TestConstraintEvaluator6) {
  using namespace taco;
  using namespace boost::assign;

  Vocabulary feature_set;
  Vocabulary value_set;

  FeatureStructureParser fs_parser(feature_set, value_set);

  OptionTable option_table;

  // Insert entries for element 0, the rule LHS.
  {
    std::vector<std::string> options;
    options += "[]";
    size_t index = 0;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  // Insert entries for element 1, "die."
  {
    std::vector<std::string> options;
    options += "[AGR:[CASE:nom;DECL:weak;NUMBER:pl];POS:ART]",
               "[AGR:[CASE:acc;DECL:weak;NUMBER:pl];POS:ART]",
               "[AGR:[CASE:nom;DECL:weak;GENDER:f;NUMBER:sg];POS:ART]",
               "[AGR:[CASE:acc;DECL:weak;GENDER:f;NUMBER:sg];POS:ART]";
    size_t index = 1;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  // Insert entries for element 3, a failed sub-derivation.
  {
    std::vector<std::string> options;
    size_t index = 3;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  // Insert entries for element 4, "Katze."
  {
    std::vector<std::string> options;
    options += "[AGR:[CASE:nom;GENDER:f;NUMBER:sg];POS:NN]",
               "[AGR:[CASE:acc;GENDER:f;NUMBER:sg];POS:NN]",
               "[AGR:[CASE:dat;GENDER:f;NUMBER:sg];POS:NN]",
               "[AGR:[CASE:gen;GENDER:f;NUMBER:sg];POS:NN]";
    size_t index = 4;
    ParseAndAddOptions(options, fs_parser, index, option_table);
  }

  BOOST_CHECK(option_table.Size() == 4);

  std::string s = std::string("<0\"AGR\">=<1\"AGR\">")
                + std::string("<0\"AGR\">=<3\"AGR\">")
                + std::string("<0\"AGR\">=<4\"AGR\">");

  ConstraintSetParser parser(feature_set, value_set);
  boost::shared_ptr<ConstraintSet> cs = parser.Parse(s);

  ConstraintEvaluator evaluator;
  std::vector<Interpretation> interpretations;
  BOOST_CHECK(!evaluator.Eval(option_table, *cs, interpretations));
  BOOST_CHECK(interpretations.size() == 0);
}

// Test for case seen during debugging.  The derivation had the following two
// rule applications:
//
//      NN -> LPs
//
//      PP-MO -> auf NN
//          <0"AGR"> = <1"AGR">
//          <0"AGR"> = <2"AGR">
//          <1"POS"> = "APPR"
//
// The first rule has no constraints and the bug relates to the previous state
// used in this case.
//
BOOST_AUTO_TEST_CASE(TestConstraintEvaluator7) {
  using namespace taco;
  using namespace boost::assign;

  Vocabulary feature_set;
  Vocabulary value_set;

  FeatureStructureParser fs_parser(feature_set, value_set);

  // Second rule application: PP-MO -> auf NN
  {
    OptionTable option_table;

    // Insert entries for element 0, the rule LHS.
    {
      std::vector<std::string> options;
      options += "[]";
      size_t index = 0;
      ParseAndAddOptions(options, fs_parser, index, option_table);
    }

    // Insert entries for element 2, "auf"
    {
      std::vector<std::string> options;
      options += "[AGR:[CASE:dat];POS:APPR]",
                 "[AGR:[CASE:acc];POS:APPR]";
      size_t index = 1;
      ParseAndAddOptions(options, fs_parser, index, option_table);
    }

    // Insert entries for element 3, "NN"
    {
      size_t index = 2;
      // The rule NN -> LPs has no constraints, so there's no evaluation
      // result.  Insert an empty feature structure.
      option_table.AddWildcardColumn(index);
    }

    BOOST_CHECK(option_table.Size() == 3);

    std::string s = std::string("<0\"AGR\">=<1\"AGR\">")
                    + std::string("<0\"AGR\">=<2\"AGR\">")
                    + std::string("<1\"POS\">=\"APPR\"");

    ConstraintSetParser parser(feature_set, value_set);
    boost::shared_ptr<ConstraintSet> cs = parser.Parse(s);

    ConstraintEvaluator evaluator;
    std::vector<Interpretation> interpretations;
    BOOST_CHECK(evaluator.Eval(option_table, *cs, interpretations));
    BOOST_CHECK(interpretations.size() == 2);
  }
}

// Test for case seen during debugging.  The derivation contained the following
// rule application:
//
//      VVPP -> ignoriert
//          <1"POS"> = "VVPP"
//          <0"SUBCAT"> = <1"SUBCAT">
//
// The lexicon contained one entry for "ignoriert": [SUBCAT: NP-OA]
//
// The problem was that the resulting set of interpretations was empty
// instead of containing the single correct interpretation:
//
//    0 -> [SUBCAT: [1]]
//
//    1 -> [POS:    VVPP
//          SUBCAT: [1] NP-OA]
//
BOOST_AUTO_TEST_CASE(TestConstraintEvaluator8) {
  using namespace taco;
  using namespace boost::assign;

  Vocabulary feature_set;
  Vocabulary value_set;

  FeatureStructureParser fs_parser(feature_set, value_set);

  // Second rule application: PP-MO -> auf NN
  {
    OptionTable option_table;

    // Insert entries for element 0, the rule LHS.
    {
      std::vector<std::string> options;
      options += "[]";
      size_t index = 0;
      ParseAndAddOptions(options, fs_parser, index, option_table);
    }

    // Insert entries for element 1, "ignoriert"
    {
      std::vector<std::string> options;
      options += "[SUBCAT:NP-OA]";
      size_t index = 1;
      ParseAndAddOptions(options, fs_parser, index, option_table);
    }

    BOOST_CHECK(option_table.Size() == 2);

    std::string s = std::string("<0\"POS\">=\"VVPP\"")
                  + std::string("<0\"SUBCAT\">=<1\"SUBCAT\">");

    ConstraintSetParser parser(feature_set, value_set);
    boost::shared_ptr<ConstraintSet> cs = parser.Parse(s);

    ConstraintEvaluator evaluator;
    std::vector<Interpretation> interpretations;
    BOOST_CHECK(evaluator.Eval(option_table, *cs, interpretations));
    BOOST_CHECK(interpretations.size() == 1);
  }
}
