#include <boost/test/unit_test.hpp>

#include "taco/text-formats/feature_structure_seq_parser.h"

#include "taco/text-formats/feature_structure_parser.h"

#include <sstream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE(TestFeatureStructureSeqParser) {
  using namespace taco;

  Vocabulary feature_set;
  Vocabulary value_set;

  // Create a few feature structure strings.
  std::string s0 = std::string("[A:[B:x];C:[]]");
  std::string s1 = std::string("[]");
  std::string s2 = std::string("[B:x]");

  // Build a sequence string out of the feature structure strings.
  std::string s = s0 + s1 + " " + s2;

  // Parse the sequence string and store the resulting FeatureStructure objects.
  std::vector<boost::shared_ptr<FeatureStructure> > seq;
  FeatureStructureSeqParser end;
  for (FeatureStructureSeqParser parser(s, feature_set, value_set);
       parser != end; ++parser) {
    seq.push_back(*parser);
  }
  BOOST_CHECK(seq.size() == 3);

  // Parse s0, s1, and s2 using FeatureStructureParser and check they match the
  // FeatureStructures in seq.
  FeatureStructureParser fs_parser(feature_set, value_set);

  BadFeatureStructureOrderer orderer;

  boost::shared_ptr<FeatureStructure> fs0 = fs_parser.Parse(s0);
  // TODO Write equality function for feature structures.
  //BOOST_CHECK(!(*fs0 == *seq[0]);
  BOOST_CHECK(!orderer(*fs0, *seq[0]));
  BOOST_CHECK(!orderer(*seq[0], *fs0));

  boost::shared_ptr<FeatureStructure> fs1 = fs_parser.Parse(s1);
  BOOST_CHECK(!orderer(*fs1, *seq[1]));
  BOOST_CHECK(!orderer(*seq[1], *fs1));

  boost::shared_ptr<FeatureStructure> fs2 = fs_parser.Parse(s2);
  BOOST_CHECK(!orderer(*fs2, *seq[2]));
  BOOST_CHECK(!orderer(*seq[2], *fs2));
}
