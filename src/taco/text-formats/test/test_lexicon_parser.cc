#include <boost/test/unit_test.hpp>

#include "taco/text-formats/lexicon_parser.h"

#include <sstream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE(TestLexiconParser) {
  using namespace taco;

  std::stringstream s;
  s << "word ||| cfs\n"
    << "word2|||cfs2\n";

  std::vector<LexiconParser::Entry> entries;

  LexiconParser end;
  for (LexiconParser parser(s); parser != end; ++parser) {
    entries.push_back(*parser);
  }

  BOOST_CHECK(entries.size() == 2);
  BOOST_CHECK(entries[0].word == "word");
  BOOST_CHECK(entries[0].fs == "cfs");
  BOOST_CHECK(entries[1].word == "word2");
  BOOST_CHECK(entries[1].fs == "cfs2");
}
