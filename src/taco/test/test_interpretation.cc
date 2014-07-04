#include <boost/test/unit_test.hpp>

#include "taco/interpretation.h"

#include "taco/constraint.h"
#include "taco/constraint_set.h"
#include "taco/feature_structure.h"
#include "taco/text-formats/feature_structure_parser.h"
#include "taco/base/vocabulary.h"

#include <boost/assign/std/set.hpp>
#include <boost/assign/std/vector.hpp>

#include <string>
#include <utility>
#include <vector>

BOOST_AUTO_TEST_CASE(TestInterpretation) {
  using namespace taco;
  using namespace boost::assign;

  Vocabulary feature_set;
  Vocabulary value_set;
  FeatureStructureParser fs_parser(feature_set, value_set);

  boost::shared_ptr<FeatureStructure> fs0, fs1;
  fs0 = fs_parser.Parse("[]");
  fs1 = fs_parser.Parse("[AGR:[CASE:nom]]");

  Feature AGR = feature_set.Lookup("AGR");

  FeaturePath path;
  path += AGR;

  Interpretation interpretation1(0, fs0);
  PotentialInterpretation tmp(interpretation1, 1, fs1);
  Interpretation interpretation2(tmp);

  // Check contents of interpretation1.
  {
    std::set<size_t> indices;
    for (Interpretation::const_iterator p = interpretation1.begin();
         p != interpretation1.end(); ++p) {
      size_t index = p->first;
      boost::shared_ptr<FeatureStructure> fs = p->second;
      indices.insert(index);
      BOOST_CHECK(fs);
      BOOST_CHECK(fs->IsComplex());
      BOOST_CHECK(fs->IsEmpty());
    }
    BOOST_CHECK(indices.size() == 1);
    BOOST_CHECK(indices.find(0) != indices.end());
  }

  // Check contents of interpretation2.
  {
    std::set<size_t> indices;
    for (Interpretation::const_iterator p = interpretation2.begin();
         p != interpretation2.end(); ++p) {
      size_t index = p->first;
      boost::shared_ptr<FeatureStructure> fs = p->second;
      indices.insert(index);
      BOOST_CHECK(fs);
      BOOST_CHECK(fs->IsComplex());
    }
    BOOST_CHECK(indices.size() == 2);
    BOOST_CHECK(indices.find(0) != indices.end());
    BOOST_CHECK(indices.find(1) != indices.end());
  }

  RelConstraint rc(PathTerm(0, path), PathTerm(1, path));

  BOOST_CHECK(interpretation1.Eval(rc));
  BOOST_CHECK(interpretation2.Eval(rc));
}

BOOST_AUTO_TEST_CASE(TestInterpretation_EvalAbsConstraint) {
  using namespace taco;
  using namespace boost::assign;

  Vocabulary feature_set;
  Vocabulary value_set;

  const Feature A = feature_set.Insert("A");
  const Feature B = feature_set.Insert("B");
  const Feature C = feature_set.Insert("C");
  const Feature D = feature_set.Insert("D");

  const AtomicValue x = value_set.Insert("x");
  const AtomicValue y = value_set.Insert("y");

  FeaturePath path1;
  path1 += A, B, C;

  AbsConstraint constraint(PathTerm(0, path1), ValueTerm(x));

  // 1: Value is present and matches.
  {
    FeatureStructureSpec spec;
    spec.content_pairs += std::make_pair(path1, x);
    boost::shared_ptr<FeatureStructure> fs(new FeatureStructure(spec));

    Interpretation interpretation(0, fs);
    BOOST_CHECK(interpretation.Eval(constraint));

    Interpretation::const_iterator p = interpretation.begin();
    boost::shared_ptr<FeatureStructure> result = p->second;
    BOOST_CHECK(result);
    BOOST_CHECK(result->IsComplex());
    boost::shared_ptr<FeatureStructure> v1 = result->Get(path1.begin(),
        path1.end());
    BOOST_CHECK(v1);
    BOOST_CHECK(v1->IsAtomic());
    BOOST_CHECK(v1->GetAtomicValue() == x);
  }

  // 2: Value is present and conflicts.
  {
    FeatureStructureSpec spec;
    spec.content_pairs += std::make_pair(path1, y);
    boost::shared_ptr<FeatureStructure> fs(new FeatureStructure(spec));

    Interpretation interpretation(0, fs);
    BOOST_CHECK(!interpretation.Eval(constraint));
  }

  // 3: Value is not present but enclosing feature structure is.
  {
    FeaturePath path2;
    path2 += A, B, D;

    FeatureStructureSpec spec;
    spec.content_pairs += std::make_pair(path2, y);
    boost::shared_ptr<FeatureStructure> fs(new FeatureStructure(spec));

    Interpretation interpretation(0, fs);
    BOOST_CHECK(interpretation.Eval(constraint));

    Interpretation::const_iterator p = interpretation.begin();
    boost::shared_ptr<FeatureStructure> result = p->second;
    BOOST_CHECK(result);
    BOOST_CHECK(result->IsComplex());
    boost::shared_ptr<FeatureStructure> v1 = result->Get(path1.begin(),
        path1.end());
    BOOST_CHECK(v1);
    BOOST_CHECK(v1->IsAtomic());
    BOOST_CHECK(v1->GetAtomicValue() == x);
    boost::shared_ptr<FeatureStructure> v2 = result->Get(path2.begin(),
        path2.end());
    BOOST_CHECK(v2);
    BOOST_CHECK(v2->IsAtomic());
    BOOST_CHECK(v2->GetAtomicValue() == y);
  }

  // 4: Value is not present and nor is enclosing feature structure.
  {
    FeatureStructureSpec spec;
    spec.content_pairs += std::make_pair(FeaturePath(), kNullAtom);

    boost::shared_ptr<FeatureStructure> fs(new FeatureStructure(spec));

    Interpretation interpretation(0, fs);
    BOOST_CHECK(interpretation.Eval(constraint));

    Interpretation::const_iterator p = interpretation.begin();
    boost::shared_ptr<FeatureStructure> result = p->second;
    BOOST_CHECK(result);
    BOOST_CHECK(result->IsComplex());
    boost::shared_ptr<FeatureStructure> v1 = result->Get(path1.begin(),
        path1.end());
    BOOST_CHECK(v1);
    BOOST_CHECK(v1->IsAtomic());
    BOOST_CHECK(v1->GetAtomicValue() == x);
  }

  // 5: Value is present and is shared.
  {
    FeaturePath path2;
    path2 += D;

    FeatureStructureSpec spec;
    spec.content_pairs += std::make_pair(path2, x);
    spec.equiv_pairs += std::make_pair(path1, path2);

    boost::shared_ptr<FeatureStructure> fs(new FeatureStructure(spec));

    Interpretation interpretation(0, fs);
    BOOST_CHECK(interpretation.Eval(constraint));

    Interpretation::const_iterator p = interpretation.begin();
    boost::shared_ptr<FeatureStructure> result = p->second;
    BOOST_CHECK(result);
    BOOST_CHECK(result->IsComplex());
    boost::shared_ptr<FeatureStructure> v1 = result->Get(path1.begin(),
        path1.end());
    BOOST_CHECK(v1);
    BOOST_CHECK(v1->IsAtomic());
    BOOST_CHECK(v1->GetAtomicValue() == x);
    boost::shared_ptr<FeatureStructure> v2 = result->Get(path2.begin(),
        path2.end());
    BOOST_CHECK(v2 == v1);
  }
}
