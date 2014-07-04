#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "taco/constraint.h"
#include "taco/base/vocabulary.h"

#include "tools-common/constraint_table.h"

BOOST_AUTO_TEST_CASE(TestBasicConstraintTableLoader) {
// fIXME
return;
  using namespace taco;
  using namespace taco::tool;

  Vocabulary vocab;
  vocab.Insert("[NP]");   // Id 0
  vocab.Insert("der");    // Id 1
  vocab.Insert("[NN]");   // Id 2
  vocab.Insert("des");    // Id 3

  std::stringstream s;
  s << "0-1-2 ||| 1 " << std::endl
    << "0-1-2-3-2 ||| 1 2" << std::endl;

  std::stringstream t;
  t << "1 ||| <1\"AGREEMENT\">=<2\"AGREEMENT\"> <1\"POS\">=\"ART\""
    << " ||| [PARAMS:[]]"
    << std::endl
    << "2 ||| <3\"AGREEMENT\">=<4\"AGREEMENT\"> <3\"POS\">=\"ART\""
    << " ||| [PARAMS:[]]"
    << std::endl;

  Vocabulary feature_set;
  Vocabulary value_set;

  ConstraintTable table;

  BasicConstraintTableLoader loader(vocab, feature_set, value_set);
  loader.Load(s, t, table);
  BOOST_CHECK(!table.empty());
  BOOST_CHECK(table.size() == 2);

  // Check first entry.
  {
    std::vector<unsigned int> key;
    key.push_back(vocab.Lookup("[NP]"));
    key.push_back(vocab.Lookup("der"));
    key.push_back(vocab.Lookup("[NN]"));

    const ConstraintSetSet *css = table.Lookup(key);
    BOOST_CHECK(css);
    BOOST_CHECK(css->size() == 1);
    const ConstraintSet &cs = **(css->begin());
    BOOST_CHECK(cs.Size() == 2);
    // TODO Check contents of constraints.
  }

  // Check second entry.
  {
    std::vector<unsigned int> key;
    key.push_back(vocab.Lookup("[NP]"));
    key.push_back(vocab.Lookup("der"));
    key.push_back(vocab.Lookup("[NN]"));
    key.push_back(vocab.Lookup("des"));
    key.push_back(vocab.Lookup("[NN]"));

    const ConstraintSetSet *css = table.Lookup(key);
    BOOST_CHECK(css);
    BOOST_CHECK(css->size() == 2);
    const ConstraintSet &cs = **(css->begin());
    BOOST_CHECK(cs.Size() == 2);
    // TODO Check contents of constraints.
  }
}
