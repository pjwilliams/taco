#include <boost/test/unit_test.hpp>

#include "taco/text-formats/feature_structure_writer.h"

#include "taco/base/vocabulary.h"

#include <boost/assign/std/set.hpp>
#include <boost/assign/std/vector.hpp>

#include <sstream>
#include <utility>
#include <vector>

BOOST_AUTO_TEST_CASE(testFeatureStructureWriter) {
  using namespace taco;
  using namespace boost::assign;

  Vocabulary feature_set;
  Vocabulary value_set;

  FeatureStructureWriter fs_writer(feature_set, value_set);

  const Feature A = feature_set.Insert("A");
  const Feature B = feature_set.Insert("B");
  const Feature C = feature_set.Insert("C");
  const Feature D = feature_set.Insert("D");
  const Feature E = feature_set.Insert("E");
  const Feature F = feature_set.Insert("F");
  const Feature G = feature_set.Insert("G");

  const AtomicValue x = value_set.Insert("x");
  const AtomicValue y = value_set.Insert("y");
  const AtomicValue z = value_set.Insert("z");
  const AtomicValue w = value_set.Insert("w");

  {
    FeatureStructureSpec spec;
    FeaturePath path1, path2, path3, path4;
    path1 += B, C, D;
    path2 += B, C, E;
    //path3 += A;
    //path4 += B, C;
    spec.content_pairs += std::make_pair(path1, x),
                          std::make_pair(path2, y);
    //spec.equiv_pairs += std::make_pair(path3, path4);

    boost::shared_ptr<FeatureStructure> fs(new FeatureStructure(spec));
    std::ostringstream out;
    fs_writer.Write(*fs, out);
    std::string s = out.str();
    BOOST_CHECK(s == "[B:[C:[D:x;E:y]]]");
  }

  {
    FeatureStructureSpec spec;
    FeaturePath path1, path2, path3, path4;
    path1 += A, D;
    path2 += A, F;
    path3 += B, C, E;
    path4 += B, G;
    spec.content_pairs += std::make_pair(path1, x),
                          std::make_pair(path2, z),
                          std::make_pair(path3, y),
                          std::make_pair(path4, w);
    boost::shared_ptr<FeatureStructure> fs(new FeatureStructure(spec));
    std::ostringstream out;
    fs_writer.Write(*fs, out);
    std::string s = out.str();
    BOOST_CHECK(s == "[A:[D:x;F:z];B:[C:[E:y];G:w]]");
  }
}
