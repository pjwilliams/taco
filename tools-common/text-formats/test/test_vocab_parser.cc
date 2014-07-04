#include <boost/test/unit_test.hpp>

#include "text-formats/vocab_parser.h"

#include <sstream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE(TestVocabParser) {
  using namespace taco::tool;

  std::stringstream s;
  s << "[NN] ||| 10544153 ||| |||" << std::endl
    << "an ||| 160304 ||| APPR ADV APZR PTKVZ ||| 160301 1 1 1" << std::endl;

  VocabParser parser(s);
  VocabParser end;

  BOOST_CHECK(parser != end);

  {
    const VocabParser::Entry &entry = *parser;
    BOOST_CHECK(entry.symbol == "[NN]");
    BOOST_CHECK(entry.count == "10544153");
    BOOST_CHECK(entry.pos_set.empty());
    BOOST_CHECK(entry.pos_counts.empty());
  }

  ++parser;
  BOOST_CHECK(parser != end);

  {
    const VocabParser::Entry &entry = *parser;
    BOOST_CHECK(entry.symbol == "an");
    BOOST_CHECK(entry.count == "160304");
    BOOST_CHECK(entry.pos_set.size() == 4);
    BOOST_CHECK(entry.pos_set[0] == "APPR");
    BOOST_CHECK(entry.pos_set[1] == "ADV");
    BOOST_CHECK(entry.pos_set[2] == "APZR");
    BOOST_CHECK(entry.pos_set[3] == "PTKVZ");
    BOOST_CHECK(entry.pos_counts.size() == 4);
    BOOST_CHECK(entry.pos_counts[0] == "160301");
    BOOST_CHECK(entry.pos_counts[1] == "1");
    BOOST_CHECK(entry.pos_counts[2] == "1");
    BOOST_CHECK(entry.pos_counts[3] == "1");
  }

  ++parser;
  BOOST_CHECK(parser == end);
}
