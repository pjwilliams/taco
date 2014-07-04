#include <boost/test/unit_test.hpp>

#include "taco/text-formats/feature_structure_parser.h"

#include "taco/feature_structure.h"
#include "taco/base/vocabulary.h"

#include <boost/assign/std/vector.hpp>

#include <string>

BOOST_AUTO_TEST_CASE(TestFeatureStructureParser) {
  using namespace taco;
  using namespace boost::assign;

  typedef boost::shared_ptr<FeatureStructure> SPFS;

  {
    std::string s = "[]";
    Vocabulary feature_set;
    Vocabulary value_set;
    FeatureStructureParser parser(feature_set, value_set);
    SPFS fs = parser.Parse(s);

    BOOST_CHECK(fs);
    BOOST_CHECK(!fs->IsAtomic());
    BOOST_CHECK(fs->IsComplex());
    BOOST_CHECK(fs->IsEmpty());
    BOOST_CHECK(fs->IsEffectivelyEmpty());
  }

  {
    std::string s = "[AGREEMENT:[]]";
    Vocabulary feature_set;
    Vocabulary value_set;
    FeatureStructureParser parser(feature_set, value_set);
    SPFS fs = parser.Parse(s);

    BOOST_CHECK(fs);
    BOOST_CHECK(!fs->IsAtomic());
    BOOST_CHECK(fs->IsComplex());
    BOOST_CHECK(!fs->IsEmpty());
    BOOST_CHECK(fs->IsEffectivelyEmpty());

    Feature agreement = feature_set.Lookup("AGREEMENT");

    SPFS value = fs->Get(agreement);

    BOOST_CHECK(value);
    BOOST_CHECK(!value->IsAtomic());
    BOOST_CHECK(value->IsComplex());
    BOOST_CHECK(value->IsEmpty());
    BOOST_CHECK(value->IsEffectivelyEmpty());
  }

  {
    std::string s = "[A:[D:x;F:z];B:[C:[E:y];G:w]]";
    Vocabulary feature_set;
    Vocabulary value_set;
    FeatureStructureParser parser(feature_set, value_set);
    SPFS fs = parser.Parse(s);

    BOOST_CHECK(fs);

    Feature A = feature_set.Lookup("A");
    Feature B = feature_set.Lookup("B");
    Feature C = feature_set.Lookup("C");
    Feature D = feature_set.Lookup("D");
    Feature E = feature_set.Lookup("E");
    Feature F = feature_set.Lookup("F");
    Feature G = feature_set.Lookup("G");

    AtomicValue w = value_set.Lookup("w");
    AtomicValue x = value_set.Lookup("x");
    AtomicValue y = value_set.Lookup("y");
    AtomicValue z = value_set.Lookup("z");

    FeaturePath path1, path2, path3, path4, path5, path6, path7;

    path1 += A;
    path2 += A, D;
    path3 += A, F;
    path4 += B;
    path5 += B, C;
    path6 += B, C, E;
    path7 += B, G;

    SPFS v1 = fs->Get(path1.begin(), path1.end());
    SPFS v2 = fs->Get(path2.begin(), path2.end());
    SPFS v3 = fs->Get(path3.begin(), path3.end());
    SPFS v4 = fs->Get(path4.begin(), path4.end());
    SPFS v5 = fs->Get(path5.begin(), path5.end());
    SPFS v6 = fs->Get(path6.begin(), path6.end());
    SPFS v7 = fs->Get(path7.begin(), path7.end());

    BOOST_CHECK(v1);
    BOOST_CHECK(v1->IsComplex());

    BOOST_CHECK(v2);
    BOOST_CHECK(v2->IsAtomic());
    BOOST_CHECK(v2->GetAtomicValue() == x);

    BOOST_CHECK(v3);
    BOOST_CHECK(v3->IsAtomic());
    BOOST_CHECK(v3->GetAtomicValue() == z);

    BOOST_CHECK(v4);
    BOOST_CHECK(v4->IsComplex());

    BOOST_CHECK(v5);
    BOOST_CHECK(v5->IsComplex());

    BOOST_CHECK(v6);
    BOOST_CHECK(v6->IsAtomic());
    BOOST_CHECK(v6->GetAtomicValue() == y);

    BOOST_CHECK(v7);
    BOOST_CHECK(v7->IsAtomic());
    BOOST_CHECK(v7->GetAtomicValue() == w);
  }
}
