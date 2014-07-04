#include <boost/test/unit_test.hpp>

#include "taco/text-formats/constraint_set_set_parser.h"

#include <sstream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE(TestConstraintSetSetParser) {
  using namespace taco;

  std::string s = std::string("<0\"AGR\"> = <1\"AGR\"> ")
                  + std::string("<0\"AGR\"> = <4\"AGR\"> ")
                  + std::string("<4\"POS\"> = \"NN\"")
                  + std::string("<0\"AGR\"\"CASE\"> = \"nom\"")
                  + std::string(":")
                  + std::string("<3> = <5>");

  Vocabulary feature_set;
  Vocabulary value_set;

  ConstraintSetSetParser parser(feature_set, value_set);

  boost::shared_ptr<ConstraintSetSet> css = parser.Parse(s);

  BOOST_CHECK(css->size() == 2);

  // Build an identical version of the first constraint set from s and check
  // that it's present in css.
  {
    boost::shared_ptr<ConstraintSet> cs(new ConstraintSet());
    {
      FeaturePath path(1, feature_set.Lookup("AGR"));
      boost::shared_ptr<RelConstraint> constraint;
      constraint.reset(new RelConstraint(PathTerm(0, path),
                                         PathTerm(1, path)));
      cs->rel_set().Insert(constraint);
    }
    {
      FeaturePath path(1, feature_set.Lookup("AGR"));
      boost::shared_ptr<RelConstraint> constraint;
      constraint.reset(new RelConstraint(PathTerm(0, path),
                                         PathTerm(4, path)));
      cs->rel_set().Insert(constraint);
    }
    {
      FeaturePath path(1, feature_set.Lookup("POS"));
      boost::shared_ptr<AbsConstraint> constraint;
      constraint.reset(new AbsConstraint(
                         PathTerm(4, path), ValueTerm(value_set.Lookup("NN"))));
      cs->abs_set().Insert(constraint);
    }
    {
      FeaturePath path;
      path.push_back(feature_set.Lookup("AGR"));
      path.push_back(feature_set.Lookup("CASE"));
      boost::shared_ptr<AbsConstraint> constraint;
      constraint.reset(new AbsConstraint(
                         PathTerm(0, path), ValueTerm(value_set.Lookup("nom"))));
      cs->abs_set().Insert(constraint);
    }
    BOOST_CHECK(css->find(cs) != css->end());
  }

  // Build an identical version of the second constraint set from s and check
  // that it's present in css.
  {
    boost::shared_ptr<ConstraintSet> cs(new ConstraintSet());
    {
      FeaturePath path;
      boost::shared_ptr<RelConstraint> constraint;
      constraint.reset(new RelConstraint(PathTerm(3, path),
                                         PathTerm(5, path)));
      cs->rel_set().Insert(constraint);
    }
    BOOST_CHECK(css->find(cs) != css->end());
  }
}
