#include <boost/test/unit_test.hpp>

#include "taco/text-formats/constraint_set_seq_parser.h"

#include "taco/text-formats/constraint_set_parser.h"

#include <sstream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE(TestConstraintSetSeqParser) {
  using namespace taco;

  Vocabulary feature_set;
  Vocabulary value_set;

  // Create a few constraint set strings.
  std::string s0 = std::string("<0\"AGR\"> = <1\"AGR\">");
  std::string s1 = std::string("<0\"AGR\"\"CASE\"> = \"nom\"");
  std::string s2 = std::string("<3>=<5>");

  // Build a sequence string out of the constraint set strings.
  std::string s = s0 + ":" + s1 + " : " + s2;

  // Parse the sequence string and store the resulting ConstraintSet objects.
  std::vector<boost::shared_ptr<ConstraintSet> > seq;
  ConstraintSetSeqParser end;
  for (ConstraintSetSeqParser parser(s, feature_set, value_set);
       parser != end; ++parser) {
    seq.push_back(*parser);
  }
  BOOST_CHECK(seq.size() == 3);

  // Parse s0, s1, and s2 using ConstraintSetParser and check they match the
  // ConstraintSets in seq.
  ConstraintSetParser cs_parser(feature_set, value_set);

  boost::shared_ptr<ConstraintSet> cs0 = cs_parser.Parse(s0);
  BOOST_CHECK(*cs0 == *seq[0]);

  boost::shared_ptr<ConstraintSet> cs1 = cs_parser.Parse(s1);
  BOOST_CHECK(*cs1 == *seq[1]);

  boost::shared_ptr<ConstraintSet> cs2 = cs_parser.Parse(s2);
  BOOST_CHECK(*cs2 == *seq[2]);
}
