#include <sstream>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "tools-common/compat-moses/rule_table_parser.h"

#include "taco/base/string_piece.h"

BOOST_AUTO_TEST_CASE(TestRuleTableParser1) {
  namespace moses = taco::tool::moses;

  std::stringstream s;
  s << "! \" [X] |||"
    << " ! [PUNC.] |||"
    << " 8.67487e-05 3.4606e-06 0.197475 0.44254 2.718 ||| ||| 22764 10"
    << std::endl
    << "[X][ADJA] [X][ADJA] agreement [X] |||"
    << " [X][ADJA] [X][ADJA] Abkommen [NP] |||"
    << " 0.0420159 0.474709 0.141054 0.178352 2.718 ||| 0-0 1-1 ||| 47.351 14.124 2.481"
    << std::endl;

  moses::RuleTableParser parser(s);
  moses::RuleTableParser end;

  BOOST_CHECK(parser != end);

  {
    const moses::RuleTableParser::Entry &entry = *parser;
    BOOST_CHECK(entry.source_lhs == "[X]");
    BOOST_CHECK(entry.source_rhs.size() == 2);
    BOOST_CHECK(entry.source_rhs[0] == "!");
    BOOST_CHECK(entry.source_rhs[1] == "\"");
    BOOST_CHECK(entry.target_lhs == "[PUNC.]");
    BOOST_CHECK(entry.target_rhs.size() == 1);
    BOOST_CHECK(entry.target_rhs[0] == "!");
    BOOST_CHECK(entry.scores.size() == 5);
    BOOST_CHECK(entry.scores[0] == "8.67487e-05");
    BOOST_CHECK(entry.scores[1] == "3.4606e-06");
    BOOST_CHECK(entry.scores[2] == "0.197475");
    BOOST_CHECK(entry.scores[3] == "0.44254");
    BOOST_CHECK(entry.scores[4] == "2.718");
    BOOST_CHECK(entry.alignment.empty());
    BOOST_CHECK(entry.counts.size() == 2);
    BOOST_CHECK(entry.counts[0] == "22764");
    BOOST_CHECK(entry.counts[1] == "10");
  }

  ++parser;
  BOOST_CHECK(parser != end);

  {
    const moses::RuleTableParser::Entry &entry = *parser;
    BOOST_CHECK(entry.source_lhs == "[X]");
    BOOST_CHECK(entry.source_rhs.size() == 3);
    BOOST_CHECK(entry.source_rhs[0] == "[X]");
    BOOST_CHECK(entry.source_rhs[1] == "[X]");
    BOOST_CHECK(entry.source_rhs[2] == "agreement");
    BOOST_CHECK(entry.target_lhs == "[NP]");
    BOOST_CHECK(entry.target_rhs.size() == 3);
    BOOST_CHECK(entry.target_rhs[0] == "[ADJA]");
    BOOST_CHECK(entry.target_rhs[1] == "[ADJA]");
    BOOST_CHECK(entry.target_rhs[2] == "Abkommen");
    BOOST_CHECK(entry.scores.size() == 5);
    BOOST_CHECK(entry.scores[0] == "0.0420159");
    BOOST_CHECK(entry.scores[1] == "0.474709");
    BOOST_CHECK(entry.scores[2] == "0.141054");
    BOOST_CHECK(entry.scores[3] == "0.178352");
    BOOST_CHECK(entry.scores[4] == "2.718");
    BOOST_CHECK(entry.alignment.size() == 2);
    BOOST_CHECK(entry.alignment[0].first == "0");
    BOOST_CHECK(entry.alignment[0].second == "0");
    BOOST_CHECK(entry.alignment[1].first == "1");
    BOOST_CHECK(entry.alignment[1].second == "1");
    BOOST_CHECK(entry.counts.size() == 3);
    BOOST_CHECK(entry.counts[0] == "47.351");
    BOOST_CHECK(entry.counts[1] == "14.124");
    BOOST_CHECK(entry.counts[2] == "2.481");
  }

  ++parser;
  BOOST_CHECK(parser == end);
}

BOOST_AUTO_TEST_CASE(TestRuleTableParser2) {
  namespace moses = taco::tool::moses;

  std::stringstream s;
  s << "in its [X][NN] , [X][subj] [X][aux] [X][punct] [X] |||"
    << " in seiner [X][NN] hat [X][subj] [X][aux] [X][punct] [sent] |||"
    << " 0.103483 0.0133483 0.0104793 0.000118263 0.000103795 0.606531 |||"
    << " 0-0 1-1 2-2 3-3 4-4 5-5 6-6 |||"
    << " 8 79 2 |||"
    << " |||"
    << " {{Tree [sent [vroot [pp [APPR in] [pn [det [PPOSAT seiner]] [NN]]] [VAFIN hat] [subj] [aux]] [punct]]}} |||"
    << " 0:3897 0:2074 |||"
    << " 1"
    << std::endl;

  const moses::RuleTableParser::Field fields = moses::RuleTableParser::ALL_CM;

  moses::RuleTableParser parser(s, fields);
  moses::RuleTableParser end;

  BOOST_CHECK(parser != end);

  {
    const moses::RuleTableParser::Entry &entry = *parser;
    BOOST_CHECK(entry.source_lhs == "[X]");
    BOOST_CHECK(entry.source_rhs.size() == 7);
    BOOST_CHECK(entry.source_rhs[0] == "in");
    BOOST_CHECK(entry.source_rhs[1] == "its");
    BOOST_CHECK(entry.source_rhs[2] == "[X]");
    BOOST_CHECK(entry.source_rhs[3] == ",");
    BOOST_CHECK(entry.source_rhs[4] == "[X]");
    BOOST_CHECK(entry.source_rhs[5] == "[X]");
    BOOST_CHECK(entry.source_rhs[6] == "[X]");
    BOOST_CHECK(entry.target_lhs == "[sent]");
    BOOST_CHECK(entry.target_rhs.size() == 7);
    BOOST_CHECK(entry.target_rhs[0] == "in");
    BOOST_CHECK(entry.target_rhs[1] == "seiner");
    BOOST_CHECK(entry.target_rhs[2] == "[NN]");
    BOOST_CHECK(entry.target_rhs[3] == "hat");
    BOOST_CHECK(entry.target_rhs[4] == "[subj]");
    BOOST_CHECK(entry.target_rhs[5] == "[aux]");
    BOOST_CHECK(entry.target_rhs[6] == "[punct]");
    BOOST_CHECK(entry.scores.size() == 6);
    BOOST_CHECK(entry.scores[0] == "0.103483");
    BOOST_CHECK(entry.scores[1] == "0.0133483");
    BOOST_CHECK(entry.scores[2] == "0.0104793");
    BOOST_CHECK(entry.scores[3] == "0.000118263");
    BOOST_CHECK(entry.scores[4] == "0.000103795");
    BOOST_CHECK(entry.scores[5] == "0.606531");
    BOOST_CHECK(entry.alignment.size() == 7);
    BOOST_CHECK(entry.alignment[0].first == "0");
    BOOST_CHECK(entry.alignment[0].second == "0");
    BOOST_CHECK(entry.alignment[1].first == "1");
    BOOST_CHECK(entry.alignment[1].second == "1");
    BOOST_CHECK(entry.alignment[2].first == "2");
    BOOST_CHECK(entry.alignment[2].second == "2");
    BOOST_CHECK(entry.alignment[3].first == "3");
    BOOST_CHECK(entry.alignment[3].second == "3");
    BOOST_CHECK(entry.alignment[4].first == "4");
    BOOST_CHECK(entry.alignment[4].second == "4");
    BOOST_CHECK(entry.alignment[5].first == "5");
    BOOST_CHECK(entry.alignment[5].second == "5");
    BOOST_CHECK(entry.alignment[6].first == "6");
    BOOST_CHECK(entry.alignment[6].second == "6");
    BOOST_CHECK(entry.counts.size() == 3);
    BOOST_CHECK(entry.counts[0] == "8");
    BOOST_CHECK(entry.counts[1] == "79");
    BOOST_CHECK(entry.counts[2] == "2");
    BOOST_CHECK(entry.sparse_features == "");
    BOOST_CHECK(entry.key_value_pairs == "{{Tree [sent [vroot [pp [APPR in] [pn [det [PPOSAT seiner]] [NN]]] [VAFIN hat] [subj] [aux]] [punct]]}}");
    BOOST_CHECK(entry.constraint_ids.size() == 2);
    BOOST_CHECK(entry.constraint_ids[0].first == "0");
    BOOST_CHECK(entry.constraint_ids[0].second == "3897");
    BOOST_CHECK(entry.constraint_ids[1].first == "0");
    BOOST_CHECK(entry.constraint_ids[1].second == "2074");
    BOOST_CHECK(entry.feature_selection_id == "1");
  }

  ++parser;
  BOOST_CHECK(parser == end);
}

BOOST_AUTO_TEST_CASE(TestRuleTableParser3) {
  namespace moses = taco::tool::moses;

  // Test entry that includes constraint IDs but not feature selection ID.
  // Normally both would be present, except during training when the constraint
  // IDs have been added but the feature selection IDs haven't.
  std::stringstream s;
  s << "! [X] ||| Art. [NN] ||| 4.06079e-05 0.0015402 4.84265e-07 3.64e-05 1 0.367879 ||| 0-0 ||| 984 82513 1 ||| ||| ||| 0:42300" << std::endl;
  std::vector<moses::RuleTableParser::Entry> entries;

  const moses::RuleTableParser::Field fields = moses::RuleTableParser::ALL_MOSES;

  moses::RuleTableParser parser(s, fields);
  moses::RuleTableParser end;

  BOOST_CHECK(parser != end);

  {
    // TODO CHECK VALUES
    //const moses::RuleTableParser::Entry &entry = *parser;
  }

  ++parser;
  BOOST_CHECK(parser == end);
}
