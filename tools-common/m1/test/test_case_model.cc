#include <sstream>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "m1/case_model.h"

BOOST_AUTO_TEST_CASE(TestCaseModel) {
  namespace m1 = taco::tool::m1;

  std::stringstream s;
  s << "OA ||| dat:0.062 nom:0.016 gen:0.007 acc:0.915 ||| 67686.000"
    << std::endl;

  std::vector<m1::CaseTableParser::Entry> entries;

  m1::CaseTableParser end;
  for (m1::CaseTableParser parser(s); parser != end; ++parser) {
    const m1::CaseTableParser::Entry &entry = *parser;
    entries.push_back(entry);
  }

  BOOST_CHECK(entries.size() == 1);

  BOOST_CHECK(entries[0].func == "OA");
  BOOST_CHECK(entries[0].probabilities.size() == 4);
  BOOST_CHECK(entries[0].probabilities["dat"] == 0.062f);
  BOOST_CHECK(entries[0].probabilities["nom"] == 0.016f);
  BOOST_CHECK(entries[0].probabilities["gen"] == 0.007f);
  BOOST_CHECK(entries[0].probabilities["acc"] == 0.915f);
  BOOST_CHECK(entries[0].count == 67686.000f);
}
