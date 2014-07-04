#include <boost/test/unit_test.hpp>

#include "taco/constraint_term.h"

#include "taco/base/vocabulary.h"

#include <boost/assign/std/vector.hpp>

#include <sstream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE(TestPathTerm) {
  using namespace taco;
  using namespace boost::assign;

  Vocabulary feature_set;

  Feature A = feature_set.Insert("A");
  Feature B = feature_set.Insert("B");

  FeaturePath path1, path2;
  path1 += A;
  path2 += A, B;

  PathTerm term1(0, path1);
  PathTerm term2(0, path2);
  PathTerm term3(1, path2);

  BOOST_CHECK(term1.index() == 0);
  BOOST_CHECK(term1.path() == path1);

  PathTermOrderer orderer;

  BOOST_CHECK(!orderer(term1, term1));
  BOOST_CHECK(orderer(term1, term2));
  BOOST_CHECK(orderer(term1, term3));

  BOOST_CHECK(!orderer(term2, term1));
  BOOST_CHECK(!orderer(term2, term2));
  BOOST_CHECK(orderer(term2, term3));

  BOOST_CHECK(!orderer(term3, term1));
  BOOST_CHECK(!orderer(term3, term2));
  BOOST_CHECK(!orderer(term3, term3));
}

BOOST_AUTO_TEST_CASE(TestVarTerm) {
  using namespace taco;

  Vocabulary value_set;

  AtomicValue nom = value_set.Insert("nom");
  AtomicValue acc = value_set.Insert("acc");
  AtomicValue gen = value_set.Insert("gen");
  AtomicValue dat = value_set.Insert("dat");

  VarTerm::ProbabilityMap prob_map1;
  prob_map1[nom] = 0.25;
  prob_map1[acc] = 0.25;
  prob_map1[gen] = 0.25;
  prob_map1[dat] = 0.25;
  VarTerm term1(prob_map1);

  VarTerm::ProbabilityMap prob_map2;
  prob_map2[nom] = 0.05;
  prob_map2[acc] = 0.05;
  prob_map2[gen] = 0.70;
  prob_map2[dat] = 0.20;
  VarTerm term2(prob_map2);

  VarTermOrderer orderer;
  BOOST_CHECK(!orderer(term1, term1));
  BOOST_CHECK(!orderer(term2, term2));
  BOOST_CHECK(orderer(term1, term2) || orderer(term2, term1));
  BOOST_CHECK(!(orderer(term1, term2) && orderer(term2, term1)));
}
