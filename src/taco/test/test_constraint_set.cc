#include <boost/test/unit_test.hpp>

#include "taco/constraint_set.h"

#include "taco/base/basic_types.h"
#include "taco/base/vocabulary.h"

#include <boost/assign/std/vector.hpp>

#include <sstream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE(TestConstraintSet) {
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

  boost::shared_ptr<RelConstraint> constraint1(new RelConstraint(term1, term1));
  boost::shared_ptr<RelConstraint> constraint2(new RelConstraint(term1, term2));
  boost::shared_ptr<RelConstraint> constraint3(new RelConstraint(term2, term1));
  boost::shared_ptr<RelConstraint> constraint4(new RelConstraint(term2, term2));

  ConstraintSet cs1;
  cs1.rel_set().Insert(constraint1);

  ConstraintSet cs2;
  cs2.rel_set().Insert(boost::shared_ptr<RelConstraint>(constraint1->Clone()));

  ConstraintSetOrderer orderer;

  BOOST_CHECK(!orderer(cs1, cs2));
  BOOST_CHECK(!orderer(cs2, cs1));
}
