#include <boost/test/unit_test.hpp>

#include "text-formats/constraint_map_parser.h"

#include <sstream>
#include <vector>

BOOST_AUTO_TEST_CASE(TestConstraintMapParser) {
  using namespace taco::tool;

  std::string key = "[NP]-der-[NN]-des-[NN]";

  std::stringstream s;
  s << key << " ||| 35 720" << std::endl;

  ConstraintMapParser end;
  ConstraintMapParser parser(s);

  BOOST_CHECK(parser != end);

  {
    const ConstraintMapParser::Entry &entry = *parser;

    BOOST_CHECK(entry.line + std::string("\n") == s.str());

    BOOST_CHECK(entry.key == key);

    BOOST_CHECK(entry.ids.size() == 2);
    BOOST_CHECK(entry.ids[0] == "35");
    BOOST_CHECK(entry.ids[1] == "720");
  }

  ++parser;
  BOOST_CHECK(parser == end);
}
