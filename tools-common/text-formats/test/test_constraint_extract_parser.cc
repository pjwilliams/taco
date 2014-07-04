#include <boost/test/unit_test.hpp>

#include "text-formats/constraint_extract_parser.h"

#include <sstream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE(TestConstraintExtractParser) {
  using namespace taco::tool;

  std::string constraint_sets = "<0\"AGR\">=<1\"AGR\"> <0\"AGR\">=<2\"AGR\"><1\"AGR\">=\"ART\" : <3\"AGR\">=<4\"AGR\"> <3\"AGR\">=\"ART\"";

  std::stringstream s;
  s << "[NP-SB] ||| der [NN] des [NN] ||| " << constraint_sets << std::endl;

  ConstraintExtractParser parser(s);
  ConstraintExtractParser end;

  BOOST_CHECK(parser != end);

  {
    const ConstraintExtractParser::Entry &entry = *parser;

    BOOST_CHECK(entry.line + std::string("\n") == s.str());

    BOOST_CHECK(entry.lhs == "[NP-SB]");

    BOOST_CHECK(entry.rhs.size() == 4);
    BOOST_CHECK(entry.rhs[0] == "der");
    BOOST_CHECK(entry.rhs[1] == "[NN]");
    BOOST_CHECK(entry.rhs[2] == "des");
    BOOST_CHECK(entry.rhs[3] == "[NN]");

    BOOST_CHECK(entry.constraint_sets == constraint_sets);
  }

  ++parser;
  BOOST_CHECK(parser == end);
}
