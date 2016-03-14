#include <boost/test/unit_test.hpp>

#include "taco/bad_feature_structure_set.h"

#include "taco/base/utility.h"
#include "taco/base/vocabulary.h"

#include <boost/assign/std/set.hpp>
#include <boost/assign/std/vector.hpp>

#include <utility>
#include <vector>

BOOST_AUTO_TEST_CASE(testBadFeatureStructureSet) {
  using namespace taco;
  using namespace boost::assign;

  Vocabulary feature_set;
  Vocabulary value_set;

  const Feature A = feature_set.Insert("A");
  const Feature B = feature_set.Insert("B");
  const Feature C = feature_set.Insert("C");
  const Feature D = feature_set.Insert("D");
  const Feature E = feature_set.Insert("E");

  const AtomicValue x = value_set.Insert("x");
  const AtomicValue y = value_set.Insert("y");

  FeatureStructureSpec spec1;
  {
    FeaturePath path1, path2, path3, path4;
    path1 += B, C, D;
    path2 += B, C, E;
    path3 += A;
    path4 += B, C;
    spec1.content_pairs += std::make_pair(path1, x),
                           std::make_pair(path2, y);
    spec1.equiv_pairs += std::make_pair(path3, path4);
  }

  boost::shared_ptr<FeatureStructure> fs1(new FeatureStructure(spec1));

  BadFeatureStructureSet fsSet;
  BOOST_CHECK(fsSet.empty());

  // Add fs1
  fsSet.insert(fs1);
  BOOST_CHECK(fsSet.size() == 1);

  // Try re-adding same shared_ptr
  fsSet.insert(fs1);
  BOOST_CHECK(fsSet.size() == 1);

  // Try adding new shared_ptr to same FS
  boost::shared_ptr<FeatureStructure> fs2(fs1);
  fsSet.insert(fs2);
  BOOST_CHECK(fsSet.size() == 1);

  // Try adding shared_ptr to FS that compares equal
  boost::shared_ptr<FeatureStructure> fs3(new FeatureStructure(spec1));
  fsSet.insert(fs3);
  BOOST_CHECK(fsSet.size() == 1);

  // Try adding shared_ptr to FS that doesn't compare equal
  FeatureStructureSpec spec2;
  {
    FeaturePath path1;
    path1 += B, C, D;
    spec2.content_pairs += std::make_pair(path1, x);
  }
  boost::shared_ptr<FeatureStructure> fs4(new FeatureStructure(spec2));
  fsSet.insert(fs4);
  BOOST_CHECK(fsSet.size() == 2);
}
