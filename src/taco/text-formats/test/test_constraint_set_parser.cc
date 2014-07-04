#include <boost/test/unit_test.hpp>

#include "taco/text-formats/constraint_set_parser.h"

#include <sstream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE(TestConstraintSetParser) {
  using namespace taco;

  std::string s = std::string("<0\"AGR\"> = <1\"AGR\"> ")
                  + std::string("<0\"AGR\"> = <4\"AGR\"> ")
                  + std::string("<4\"POS\"> = \"NN\"")
                  + std::string("<0\"AGR\"\"CASE\"> = \"nom\"")
                  + std::string("<3> = <5>");

  Vocabulary feature_set;
  Vocabulary value_set;

  ConstraintSetParser parser(feature_set, value_set);

  boost::shared_ptr<ConstraintSet> cs = parser.Parse(s);

  BOOST_CHECK(cs.get());

  BOOST_CHECK(cs->Size() == 5);

  {
    FeaturePath path(1, feature_set.Lookup("AGR"));
    boost::shared_ptr<RelConstraint> constraint;
    constraint.reset(new RelConstraint(PathTerm(0, path),
                                       PathTerm(1, path)));
    BOOST_CHECK(cs->rel_set().Find(constraint) != cs->rel_set().End());
  }

  {
    FeaturePath path(1, feature_set.Lookup("AGR"));
    boost::shared_ptr<RelConstraint> constraint;
    constraint.reset(new RelConstraint(PathTerm(0, path),
                                       PathTerm(4, path)));
    BOOST_CHECK(cs->rel_set().Find(constraint) != cs->rel_set().End());
  }

  {
    FeaturePath path(1, feature_set.Lookup("POS"));
    boost::shared_ptr<AbsConstraint> constraint;
    constraint.reset(new AbsConstraint(PathTerm(4, path),
                                       ValueTerm(value_set.Lookup("NN"))));
    BOOST_CHECK(cs->abs_set().Find(constraint) != cs->abs_set().End());
  }

  {
    FeaturePath path;
    path.push_back(feature_set.Lookup("AGR"));
    path.push_back(feature_set.Lookup("CASE"));
    boost::shared_ptr<AbsConstraint> constraint;
    constraint.reset(new AbsConstraint(PathTerm(0, path),
                                       ValueTerm(value_set.Lookup("nom"))));
    BOOST_CHECK(cs->abs_set().Find(constraint) != cs->abs_set().End());
  }

  {
    FeaturePath path;
    boost::shared_ptr<RelConstraint> constraint;
    constraint.reset(new RelConstraint(PathTerm(3, path), PathTerm(5, path)));
    BOOST_CHECK(cs->rel_set().Find(constraint) != cs->rel_set().End());
  }
}
