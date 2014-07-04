#include <boost/test/unit_test.hpp>

#include "taco/text-formats/constraint_parser.h"

#include <sstream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE(TestConstraintParser) {
  using namespace taco;

  Vocabulary feature_set;
  Vocabulary value_set;

  ConstraintParser parser(feature_set, value_set);

  {
    std::string s = std::string("<0\"AGR\"> = <1\"AGR\">");
    ConstraintType type;
    boost::shared_ptr<Constraint> constraint = parser.Parse(s, type);
    BOOST_CHECK(type == kRelConstraint);
    RelConstraint *rc = static_cast<RelConstraint *>(constraint.get());
    BOOST_CHECK(rc);
    BOOST_CHECK(rc->lhs.index() == 0);
    BOOST_CHECK(rc->lhs.path().size() == 1);
    BOOST_CHECK(rc->lhs.path()[0] == feature_set.Lookup("AGR"));
    BOOST_CHECK(rc->rhs.index() == 1);
    BOOST_CHECK(rc->rhs.path().size() == 1);
    BOOST_CHECK(rc->rhs.path()[0] == feature_set.Lookup("AGR"));
  }

  {
    std::string s = std::string("<1\"AGR\"\"CASE\"> = {\"nom\":0.058,\"acc\":0.006,\"gen\":0.730,\"dat\":0.206}");
    ConstraintType type;
    boost::shared_ptr<Constraint> constraint = parser.Parse(s, type);
    BOOST_CHECK(type == kVarConstraint);
    VarConstraint *vc = static_cast<VarConstraint *>(constraint.get());
    BOOST_CHECK(vc);
    BOOST_CHECK(vc->lhs.index() == 1);
    BOOST_CHECK(vc->lhs.path().size() == 2);
    BOOST_CHECK(vc->lhs.path()[0] == feature_set.Lookup("AGR"));
    BOOST_CHECK(vc->lhs.path()[1] == feature_set.Lookup("CASE"));
    AtomicValue nom = value_set.Lookup("nom");
    AtomicValue acc = value_set.Lookup("acc");
    AtomicValue gen = value_set.Lookup("gen");
    AtomicValue dat = value_set.Lookup("dat");
    BOOST_CHECK(vc->rhs.probabilities().size() == 4);
    VarTerm::ProbabilityMap::const_iterator p;
    // Check nom probability.
    p = vc->rhs.probabilities().find(nom);
    BOOST_CHECK(p != vc->rhs.probabilities().end());
    BOOST_CHECK(p->second == 0.058f);
    // Check acc probability.
    p = vc->rhs.probabilities().find(acc);
    BOOST_CHECK(p != vc->rhs.probabilities().end());
    BOOST_CHECK(p->second == 0.006f);
    // Check gen probability.
    p = vc->rhs.probabilities().find(gen);
    BOOST_CHECK(p != vc->rhs.probabilities().end());
    BOOST_CHECK(p->second == 0.730f);
    // Check dat probability.
    p = vc->rhs.probabilities().find(dat);
    BOOST_CHECK(p != vc->rhs.probabilities().end());
    BOOST_CHECK(p->second == 0.206f);
  }
}
