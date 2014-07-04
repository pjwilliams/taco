#include <boost/test/unit_test.hpp>

#include "taco/constraint.h"

#include "taco/base/vocabulary.h"

#include <boost/assign/std/vector.hpp>

#include <sstream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE(TestAbsConstraint) {
  using namespace taco;
  using namespace boost::assign;

  Vocabulary feature_set;
  Vocabulary value_set;

  Feature A = feature_set.Insert("A");
  Feature B = feature_set.Insert("B");

  AtomicValue x = value_set.Insert("x");
  AtomicValue y = value_set.Insert("y");

  FeaturePath path1, path2;
  path1 += A;
  path2 += A, B;

  PathTerm term1(0, path1);
  PathTerm term2(0, path2);
  PathTerm term3(1, path2);

  AbsConstraint constraint1(term1, x);
  AbsConstraint constraint2(term1, y);
  AbsConstraint constraint3(term2, x);
  AbsConstraint constraint4(term2, y);

  AbsConstraintOrderer orderer;

  BOOST_CHECK(!orderer(constraint1, constraint1));
  BOOST_CHECK(orderer(constraint1, constraint2));
  BOOST_CHECK(orderer(constraint1, constraint3));
  BOOST_CHECK(orderer(constraint1, constraint4));

  BOOST_CHECK(!orderer(constraint2, constraint1));
  BOOST_CHECK(!orderer(constraint2, constraint2));
  BOOST_CHECK(orderer(constraint2, constraint3));
  BOOST_CHECK(orderer(constraint2, constraint4));

  BOOST_CHECK(!orderer(constraint3, constraint1));
  BOOST_CHECK(!orderer(constraint3, constraint2));
  BOOST_CHECK(!orderer(constraint3, constraint3));
  BOOST_CHECK(orderer(constraint3, constraint4));

  BOOST_CHECK(!orderer(constraint4, constraint1));
  BOOST_CHECK(!orderer(constraint4, constraint2));
  BOOST_CHECK(!orderer(constraint4, constraint3));
  BOOST_CHECK(!orderer(constraint4, constraint4));
}

BOOST_AUTO_TEST_CASE(TestRelConstraint) {
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

  RelConstraint constraint1(term1, term1);
  RelConstraint constraint2(term1, term2);
  RelConstraint constraint3(term2, term1);
  RelConstraint constraint4(term2, term2);

  RelConstraintOrderer orderer;

  BOOST_CHECK(!orderer(constraint1, constraint1));
  BOOST_CHECK(orderer(constraint1, constraint2));
  BOOST_CHECK(orderer(constraint1, constraint3));
  BOOST_CHECK(orderer(constraint1, constraint4));

  BOOST_CHECK(!orderer(constraint2, constraint1));
  BOOST_CHECK(!orderer(constraint2, constraint2));
  BOOST_CHECK(!orderer(constraint2, constraint3));
  BOOST_CHECK(orderer(constraint2, constraint4));

  BOOST_CHECK(!orderer(constraint3, constraint1));
  BOOST_CHECK(!orderer(constraint3, constraint2));
  BOOST_CHECK(!orderer(constraint3, constraint3));
  BOOST_CHECK(orderer(constraint3, constraint4));

  BOOST_CHECK(!orderer(constraint4, constraint1));
  BOOST_CHECK(!orderer(constraint4, constraint2));
  BOOST_CHECK(!orderer(constraint4, constraint3));
  BOOST_CHECK(!orderer(constraint4, constraint4));
}
