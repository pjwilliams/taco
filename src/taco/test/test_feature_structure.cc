#include <boost/test/unit_test.hpp>

#include "taco/feature_structure.h"

#include "taco/base/utility.h"
#include "taco/base/vocabulary.h"

#include <boost/assign/std/set.hpp>
#include <boost/assign/std/vector.hpp>

#include <utility>
#include <vector>

// Tests constructions, successful unification, and cloning.
BOOST_AUTO_TEST_CASE(TestFeatureStructure) {
  using namespace taco;
  using namespace boost::assign;

  typedef boost::shared_ptr<FeatureStructure> SPFS;

  Vocabulary feature_set;
  Vocabulary value_set;

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

  SPFS fs1(new FeatureStructure(spec1));

  {
    BOOST_CHECK(!fs1->IsAtomic());
    BOOST_CHECK(fs1->IsComplex());
    BOOST_CHECK(!fs1->IsEmpty());
    BOOST_CHECK(!fs1->IsEffectivelyEmpty());

    FeaturePath path1, path2, path3, path4;

    path1 += A;
    path2 += A, D;
    path3 += B, C;
    path4 += B, C, D;

    SPFS v1 = fs1->Get(path1.begin(), path1.end());
    SPFS v2 = fs1->Get(path2.begin(), path2.end());
    SPFS v3 = fs1->Get(path3.begin(), path3.end());
    SPFS v4 = fs1->Get(path4.begin(), path4.end());

    BOOST_CHECK(v1);
    BOOST_CHECK(v1->IsComplex());

    BOOST_CHECK(v2);
    BOOST_CHECK(v2->IsAtomic());
    BOOST_CHECK(v2->GetAtomicValue() == x);

    BOOST_CHECK(v3);
    BOOST_CHECK(v3 == v1);

    BOOST_CHECK(v4);
    BOOST_CHECK(v4 == v2);
  }

  FeatureStructureSpec spec2;
  {
    FeaturePath path1, path2, path3, path4;
    path1 += A, D;
    path2 += A, F;
    path3 += B, C, E;
    path4 += B, G;
    spec2.content_pairs += std::make_pair(path1, x),
                          std::make_pair(path2, z),
                          std::make_pair(path3, y),
                          std::make_pair(path4, w);
  }

  SPFS fs2(new FeatureStructure(spec2));

  {
    BOOST_CHECK(!fs2->IsAtomic());
    BOOST_CHECK(fs2->IsComplex());
    BOOST_CHECK(!fs2->IsEmpty());
    BOOST_CHECK(!fs2->IsEffectivelyEmpty());

    FeaturePath path1, path2, path3, path4, path5, path6, path7;

    path1 += A;
    path2 += A, D;
    path3 += A, F;
    path4 += B;
    path5 += B, C;
    path6 += B, C, E;
    path7 += B, G;

    SPFS v1 = fs2->Get(path1.begin(), path1.end());
    SPFS v2 = fs2->Get(path2.begin(), path2.end());
    SPFS v3 = fs2->Get(path3.begin(), path3.end());
    SPFS v4 = fs2->Get(path4.begin(), path4.end());
    SPFS v5 = fs2->Get(path5.begin(), path5.end());
    SPFS v6 = fs2->Get(path6.begin(), path6.end());
    SPFS v7 = fs2->Get(path7.begin(), path7.end());

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

  BOOST_CHECK(fs1->PossiblyUnifiable(fs2));
  BOOST_CHECK(fs2->PossiblyUnifiable(fs1));
  bool result = FeatureStructure::Unify(fs1, fs2);

  {
    BOOST_CHECK(result);

    BOOST_CHECK(!fs1->IsAtomic());
    BOOST_CHECK(fs1->IsComplex());
    BOOST_CHECK(!fs1->IsEmpty());
    BOOST_CHECK(!fs1->IsEffectivelyEmpty());

    FeaturePath path1, path2, path3, path4, path5, path6;

    path1 += A;
    path2 += B, C;
    path3 += A, D;
    path4 += B, C, D;
    path5 += A, F;
    path6 += B, G;

    SPFS v1 = fs1->Get(path1.begin(), path1.end());
    SPFS v2 = fs1->Get(path2.begin(), path2.end());
    SPFS v3 = fs1->Get(path3.begin(), path3.end());
    SPFS v4 = fs1->Get(path4.begin(), path4.end());
    SPFS v5 = fs1->Get(path5.begin(), path5.end());
    SPFS v6 = fs1->Get(path6.begin(), path6.end());

    BOOST_CHECK(v1);
    BOOST_CHECK(v1->IsComplex());

    BOOST_CHECK(v2 == v1);

    BOOST_CHECK(v3);
    BOOST_CHECK(v3->IsAtomic());
    BOOST_CHECK(v3->GetAtomicValue() == x);

    BOOST_CHECK(v4 == v3);

    BOOST_CHECK(v5);
    BOOST_CHECK(v5->IsAtomic());
    BOOST_CHECK(v5->GetAtomicValue() == z);

    BOOST_CHECK(v6);
    BOOST_CHECK(v6->IsAtomic());
    BOOST_CHECK(v6->GetAtomicValue() == w);

    BOOST_CHECK(!fs2->IsAtomic());
    BOOST_CHECK(fs2->IsComplex());
    BOOST_CHECK(!fs2->IsEmpty());
    BOOST_CHECK(!fs2->IsEffectivelyEmpty());

    SPFS w1 = fs2->Get(path1.begin(), path1.end());
    SPFS w2 = fs2->Get(path2.begin(), path2.end());
    SPFS w3 = fs2->Get(path3.begin(), path3.end());
    SPFS w4 = fs2->Get(path4.begin(), path4.end());
    SPFS w5 = fs2->Get(path5.begin(), path5.end());
    SPFS w6 = fs2->Get(path6.begin(), path6.end());

    BOOST_CHECK(v1 == w1);
    BOOST_CHECK(v2 == w2);
    BOOST_CHECK(v3 == w3);
    BOOST_CHECK(v4 == w4);
    BOOST_CHECK(v5 == w5);
    BOOST_CHECK(v6 == w6);
  }

  // Unify fs1 and fs2 a second time.  This should leave them unchanged.

  BOOST_CHECK(fs1->PossiblyUnifiable(fs2));
  BOOST_CHECK(fs2->PossiblyUnifiable(fs1));

  result = FeatureStructure::Unify(fs1, fs2);

  {
    BOOST_CHECK(result);

    BOOST_CHECK(!fs1->IsAtomic());
    BOOST_CHECK(fs1->IsComplex());
    BOOST_CHECK(!fs1->IsEmpty());
    BOOST_CHECK(!fs1->IsEffectivelyEmpty());

    FeaturePath path1, path2, path3, path4, path5, path6;

    path1 += A;
    path2 += B, C;
    path3 += A, D;
    path4 += B, C, D;
    path5 += A, F;
    path6 += B, G;

    SPFS v1 = fs1->Get(path1.begin(), path1.end());
    SPFS v2 = fs1->Get(path2.begin(), path2.end());
    SPFS v3 = fs1->Get(path3.begin(), path3.end());
    SPFS v4 = fs1->Get(path4.begin(), path4.end());
    SPFS v5 = fs1->Get(path5.begin(), path5.end());
    SPFS v6 = fs1->Get(path6.begin(), path6.end());

    BOOST_CHECK(v1);
    BOOST_CHECK(v1->IsComplex());

    BOOST_CHECK(v2 == v1);

    BOOST_CHECK(v3);
    BOOST_CHECK(v3->IsAtomic());
    BOOST_CHECK(v3->GetAtomicValue() == x);

    BOOST_CHECK(v4 == v3);

    BOOST_CHECK(v5);
    BOOST_CHECK(v5->IsAtomic());
    BOOST_CHECK(v5->GetAtomicValue() == z);

    BOOST_CHECK(v6);
    BOOST_CHECK(v6->IsAtomic());
    BOOST_CHECK(v6->GetAtomicValue() == w);

    BOOST_CHECK(!fs2->IsAtomic());
    BOOST_CHECK(fs2->IsComplex());
    BOOST_CHECK(!fs2->IsEmpty());
    BOOST_CHECK(!fs2->IsEffectivelyEmpty());

    SPFS w1 = fs2->Get(path1.begin(), path1.end());
    SPFS w2 = fs2->Get(path2.begin(), path2.end());
    SPFS w3 = fs2->Get(path3.begin(), path3.end());
    SPFS w4 = fs2->Get(path4.begin(), path4.end());
    SPFS w5 = fs2->Get(path5.begin(), path5.end());
    SPFS w6 = fs2->Get(path6.begin(), path6.end());

    BOOST_CHECK(v1 == w1);
    BOOST_CHECK(v2 == w2);
    BOOST_CHECK(v3 == w3);
    BOOST_CHECK(v4 == w4);
    BOOST_CHECK(v5 == w5);
    BOOST_CHECK(v6 == w6);
  }

  SPFS clone = fs1->Clone();
  fs1.reset();

  {
    BOOST_CHECK(!clone->IsAtomic());
    BOOST_CHECK(clone->IsComplex());
    BOOST_CHECK(!clone->IsEmpty());
    BOOST_CHECK(!clone->IsEffectivelyEmpty());

    FeaturePath path1, path2, path3, path4, path5, path6;

    path1 += A;
    path2 += B, C;
    path3 += A, D;
    path4 += B, C, D;
    path5 += A, F;
    path6 += B, G;

    SPFS v1 = clone->Get(path1.begin(), path1.end());
    SPFS v2 = clone->Get(path2.begin(), path2.end());
    SPFS v3 = clone->Get(path3.begin(), path3.end());
    SPFS v4 = clone->Get(path4.begin(), path4.end());
    SPFS v5 = clone->Get(path5.begin(), path5.end());
    SPFS v6 = clone->Get(path6.begin(), path6.end());

    BOOST_CHECK(v1);
    BOOST_CHECK(v1->IsComplex());

    BOOST_CHECK(v2 == v1);

    BOOST_CHECK(v3);
    BOOST_CHECK(v3->IsAtomic());
    BOOST_CHECK(v3->GetAtomicValue() == x);

    BOOST_CHECK(v4 == v3);

    BOOST_CHECK(v5);
    BOOST_CHECK(v5->IsAtomic());
    BOOST_CHECK(v5->GetAtomicValue() == z);

    BOOST_CHECK(v6);
    BOOST_CHECK(v6->IsAtomic());
    BOOST_CHECK(v6->GetAtomicValue() == w);
  }
}

// Tests distinction between actual and effective emptiness.
BOOST_AUTO_TEST_CASE(TestFeatureStructure2) {
  using namespace taco;
  using namespace boost::assign;

  typedef boost::shared_ptr<FeatureStructure> SPFS;

  Vocabulary feature_set;

  const Feature A = feature_set.Insert("A");
  const Feature B = feature_set.Insert("B");
  const Feature C = feature_set.Insert("C");
  const Feature D = feature_set.Insert("D");

  // Create completely empty feature structure
  FeatureStructureSpec spec1;
  spec1.content_pairs.insert(std::make_pair(FeaturePath(), kNullAtom));
  SPFS fs1(new FeatureStructure(spec1));
  BOOST_CHECK(!fs1->IsAtomic());
  BOOST_CHECK(fs1->IsComplex());
  BOOST_CHECK(fs1->IsEmpty());
  BOOST_CHECK(fs1->IsEffectivelyEmpty());

  // Create 'effectively empty' feature structure
  FeatureStructureSpec spec2;
  {
    FeaturePath path1, path2;
    path1 += A;
    path2 += B, C, D;
    spec2.content_pairs += std::make_pair(path1, kNullAtom),
                           std::make_pair(path2, kNullAtom);
  }
  SPFS fs2(new FeatureStructure(spec2));
  BOOST_CHECK(!fs2->IsAtomic());
  BOOST_CHECK(fs2->IsComplex());
  BOOST_CHECK(!fs2->IsEmpty());
  BOOST_CHECK(fs2->IsEffectivelyEmpty());

  // Create 'effectively empty' feature structure with shared values
  FeatureStructureSpec spec3;
  {
    FeaturePath path1, path2;
    path1 += A;
    path2 += B, C, D;
    spec3.content_pairs += std::make_pair(path1, kNullAtom);
    spec3.equiv_pairs += std::make_pair(path1, path2);
  }
  SPFS fs3(new FeatureStructure(spec3));
  BOOST_CHECK(!fs3->IsAtomic());
  BOOST_CHECK(fs3->IsComplex());
  BOOST_CHECK(!fs3->IsEmpty());
  BOOST_CHECK(fs3->IsEffectivelyEmpty());
}

// Tests unsuccessful unification.  Examples are taken from Wroblewski (1987).
BOOST_AUTO_TEST_CASE(TestFeatureStructure3) {
  using namespace taco;
  using namespace boost::assign;

  typedef boost::shared_ptr<FeatureStructure> SPFS;

  Vocabulary feature_set;
  Vocabulary value_set;

  const Feature A = feature_set.Insert("A");
  const Feature C = feature_set.Insert("C");
  const Feature E = feature_set.Insert("E");
  const Feature X = feature_set.Insert("X");

  const AtomicValue d = value_set.Insert("d");
  const AtomicValue e = value_set.Insert("e");
  const AtomicValue y = value_set.Insert("y");

  // [C: d] + [C: e]
  {
    FeatureStructureSpec spec1;
    {
      FeaturePath path;
      path += C;
      spec1.content_pairs += std::make_pair(path, d);
    }
    SPFS fs1(new FeatureStructure(spec1));
    FeatureStructureSpec spec2;
    {
      FeaturePath path;
      path += C;
      spec2.content_pairs += std::make_pair(path, e);
    }
    SPFS fs2(new FeatureStructure(spec2));
    BOOST_CHECK(!FeatureStructure::Unify(fs1, fs2));
  }

  // [A: 1[X:y]  +  [A: [C:d]
  //  E: <1>]        E: [C:e]]
  {
    FeatureStructureSpec spec1;
    {
      FeaturePath path1, path2, path3;
      path1 += A, X;
      path2 += A;
      path3 += E;
      spec1.content_pairs += std::make_pair(path1, y);
      spec1.equiv_pairs += std::make_pair(path2, path3);
    }
    SPFS fs1(new FeatureStructure(spec1));
    FeatureStructureSpec spec2;
    {
      FeaturePath path1, path2;
      path1 += A, C;
      path2 += E, C;
      spec2.content_pairs += std::make_pair(path1, d),
                            std::make_pair(path2, e);
    }
    SPFS fs2(new FeatureStructure(spec2));
    BOOST_CHECK(!FeatureStructure::Unify(fs1, fs2));
  }
}

// Another test for successful unification.  Example taken from Jurafsky
// and Martin (2008), sec 15.4.2.
BOOST_AUTO_TEST_CASE(TestFeatureStructure4) {
  using namespace taco;
  using namespace boost::assign;

  typedef boost::shared_ptr<FeatureStructure> SPFS;

  Vocabulary feature_set;
  Vocabulary value_set;

  const Feature AGREEMENT = feature_set.Insert("AGREEMENT");
  const Feature NUMBER = feature_set.Insert("NUMBER");
  const Feature PERSON = feature_set.Insert("PERSON");
  const Feature SUBJECT = feature_set.Insert("SUBJECT");

  const AtomicValue sg = value_set.Insert("sg");
  const AtomicValue third = value_set.Insert("3rd");

  // [AGREEMENT: 1[NUMBER:sg]
  //  SUBJECT: [AGREEMENT:<1>]]
  FeatureStructureSpec spec1;
  {
    FeaturePath path1, path2, path3;
    path1 += AGREEMENT;
    path2 += AGREEMENT, NUMBER;
    path3 += SUBJECT, AGREEMENT;
    spec1.content_pairs += std::make_pair(path2, sg);
    spec1.equiv_pairs += std::make_pair(path1, path3);
  }
  SPFS fs1(new FeatureStructure(spec1));

  // [SUBJECT: [AGREEMENT: [PERSON:3rd]]]
  FeatureStructureSpec spec2;
  {
    FeaturePath path1;
    path1 += SUBJECT, AGREEMENT, PERSON;
    spec2.content_pairs += std::make_pair(path1, third);
  }
  SPFS fs2(new FeatureStructure(spec2));

  // Perform unification
  BOOST_CHECK(fs1->PossiblyUnifiable(fs2));
  BOOST_CHECK(fs2->PossiblyUnifiable(fs1));
  BOOST_CHECK(FeatureStructure::Unify(fs1, fs2));

  // Check resulting feature structure content.

  BOOST_CHECK(!fs1->IsAtomic());
  BOOST_CHECK(fs1->IsComplex());
  BOOST_CHECK(!fs1->IsEmpty());
  BOOST_CHECK(!fs1->IsEffectivelyEmpty());

  FeaturePath path1, path2, path3, path4, path5;

  path1 += AGREEMENT;
  path2 += AGREEMENT, NUMBER;
  path3 += AGREEMENT, PERSON;
  path4 += SUBJECT;
  path5 += SUBJECT, AGREEMENT;

  SPFS v1 = fs1->Get(path1.begin(), path1.end());
  SPFS v2 = fs1->Get(path2.begin(), path2.end());
  SPFS v3 = fs1->Get(path3.begin(), path3.end());
  SPFS v4 = fs1->Get(path4.begin(), path4.end());
  SPFS v5 = fs1->Get(path5.begin(), path5.end());

  BOOST_CHECK(v1);
  BOOST_CHECK(v1->IsComplex());

  BOOST_CHECK(v5 == v1);

  BOOST_CHECK(v2);
  BOOST_CHECK(v2->IsAtomic());
  BOOST_CHECK(v2->GetAtomicValue() == sg);

  BOOST_CHECK(v3);
  BOOST_CHECK(v3->IsAtomic());
  BOOST_CHECK(v3->GetAtomicValue() == third);

  BOOST_CHECK(v4);
  BOOST_CHECK(v4->IsComplex());

  SPFS w1 = fs2->Get(path1.begin(), path1.end());
  SPFS w2 = fs2->Get(path2.begin(), path2.end());
  SPFS w3 = fs2->Get(path3.begin(), path3.end());
  SPFS w4 = fs2->Get(path4.begin(), path4.end());
  SPFS w5 = fs2->Get(path5.begin(), path5.end());

  BOOST_CHECK(v1 == w1);
  BOOST_CHECK(v2 == w2);
  BOOST_CHECK(v3 == w3);
  BOOST_CHECK(v4 == w4);
  BOOST_CHECK(v5 == w5);
}

// Another test for successful unification.  Example was a problem case
// found during debugging.
BOOST_AUTO_TEST_CASE(TestFeatureStructure5) {
  using namespace taco;
  using namespace boost::assign;

  typedef boost::shared_ptr<FeatureStructure> SPFS;

  Vocabulary value_set;

  const AtomicValue np_oa = value_set.Insert("NP-OA");

  // Empty feature structure
  FeatureStructureSpec spec1;
  spec1.content_pairs.insert(std::make_pair(FeaturePath(), kNullAtom));
  SPFS fs1(new FeatureStructure(spec1));

  // Atomic feature structure with value NP-OA
  FeatureStructureSpec spec2;
  spec2.content_pairs.insert(std::make_pair(FeaturePath(), np_oa));
  SPFS fs2(new FeatureStructure(spec2));

  // Test unification
  BOOST_CHECK(fs1->PossiblyUnifiable(fs2));
  BOOST_CHECK(fs2->PossiblyUnifiable(fs1));
  BOOST_CHECK(FeatureStructure::Unify(fs1, fs2));

  BOOST_CHECK(fs1->IsAtomic());
  BOOST_CHECK(!fs1->IsComplex());
  BOOST_CHECK(!fs1->IsEmpty());
  BOOST_CHECK(!fs1->IsEffectivelyEmpty());

  BOOST_CHECK(fs1->GetAtomicValue() == np_oa);
}

// Another test for successful unification.  This one is example 3.6 from 
// Francez and Wintner (2011).
BOOST_AUTO_TEST_CASE(TestFeatureStructure6) {
  using namespace taco;
  using namespace boost::assign;

  typedef boost::shared_ptr<FeatureStructure> SPFS;

  Vocabulary feature_set;
  Vocabulary value_set;

  const Feature SUBJ = feature_set.Insert("SUBJ");
  const Feature OBJ  = feature_set.Insert("OBJ");
  const Feature NUM  = feature_set.Insert("NUM");
  const Feature PERS = feature_set.Insert("PERS");

  const AtomicValue sg = value_set.Insert("sg");
  const AtomicValue third = value_set.Insert("3rd");

  // [SUBJ: [NUM:  sg ]] + [SUBJ: 1[]]
  // [OBJ:  [PERS: 3rd]]   [OBJ:  <1>]

  FeatureStructureSpec spec1;
  {
    FeaturePath path1, path2;
    path1 += SUBJ, NUM;
    path2 += OBJ, PERS;
    spec1.content_pairs += std::make_pair(path1, sg),
                           std::make_pair(path2, third);
  }
  SPFS fs1(new FeatureStructure(spec1));

  FeatureStructureSpec spec2;
  {
    FeaturePath path1, path2;
    path1 += SUBJ;
    path2 += OBJ;
    spec2.content_pairs += std::make_pair(path1, kNullAtom);
    spec2.equiv_pairs += std::make_pair(path1, path2);
  }
  SPFS fs2(new FeatureStructure(spec2));

  // Test unification
  BOOST_CHECK(fs1->PossiblyUnifiable(fs2));
  BOOST_CHECK(fs2->PossiblyUnifiable(fs1));
  BOOST_CHECK(FeatureStructure::Unify(fs1, fs2));

  BOOST_CHECK(fs1->IsComplex());
  BOOST_CHECK(!fs1->IsEmpty());
  BOOST_CHECK(!fs1->IsEffectivelyEmpty());

  SPFS fs3 = fs1->Get(SUBJ);
  BOOST_CHECK(fs3->IsComplex());
  BOOST_CHECK(!fs3->IsEmpty());
  BOOST_CHECK(!fs3->IsEffectivelyEmpty());

  SPFS fs4 = fs3->Get(NUM);
  BOOST_CHECK(fs4->IsAtomic());
  BOOST_CHECK(fs4->GetAtomicValue() == sg);

  SPFS fs5 = fs3->Get(PERS);
  BOOST_CHECK(fs5->IsAtomic());
  BOOST_CHECK(fs5->GetAtomicValue() == third);

  SPFS fs6 = fs1->Get(OBJ);
  BOOST_CHECK(fs3.get() == fs6.get());
}

// Another test for successful unification.  This tests unification of
// an empty value with an atom.
BOOST_AUTO_TEST_CASE(TestFeatureStructure7) {
  using namespace taco;
  using namespace boost::assign;

  typedef boost::shared_ptr<FeatureStructure> SPFS;

  Vocabulary value_set;

  const AtomicValue t = value_set.Insert("t");

  FeaturePath empty_path;

  FeatureStructureSpec spec1;
  spec1.content_pairs += std::make_pair(empty_path, t);
  SPFS fs1(new FeatureStructure(spec1));

  FeatureStructureSpec spec2;
  spec2.content_pairs += std::make_pair(empty_path, kNullAtom);
  SPFS fs2(new FeatureStructure(spec2));

  // Test unification
  BOOST_CHECK(fs1->PossiblyUnifiable(fs2));
  BOOST_CHECK(fs2->PossiblyUnifiable(fs1));
  BOOST_CHECK(FeatureStructure::Unify(fs1, fs2));

  BOOST_CHECK(!fs1->IsComplex());
  BOOST_CHECK(!fs1->IsEmpty());
  BOOST_CHECK(!fs1->IsEffectivelyEmpty());

  BOOST_CHECK(fs1->IsAtomic());
  BOOST_CHECK(fs1->GetAtomicValue() == t);
}

BOOST_AUTO_TEST_CASE(TestFeatureStructureOrderer) {
  using namespace taco;
  using namespace boost::assign;

  typedef boost::shared_ptr<FeatureStructure> SPFS;

  Vocabulary feature_set;
  Vocabulary value_set;

  const Feature A = feature_set.Insert("A");
  const Feature B = feature_set.Insert("B");

  const AtomicValue x = value_set.Insert("x");
  const AtomicValue y = value_set.Insert("y");
  const AtomicValue z = value_set.Insert("z");

  SPFS fs1, fs2, fs3, fs4, fs5;

  {
    FeatureStructureSpec spec;
    FeaturePath path1;
    spec.content_pairs += std::make_pair(path1, x);
    fs1.reset(new FeatureStructure(spec));
  }

  {
    FeatureStructureSpec spec;
    FeaturePath path1;
    path1 += A;
    spec.content_pairs += std::make_pair(path1, x);
    fs2.reset(new FeatureStructure(spec));
  }

  {
    FeatureStructureSpec spec;
    FeaturePath path1, path2;
    path1 += A;
    path2 += B;
    spec.content_pairs += std::make_pair(path1, x);
    spec.content_pairs += std::make_pair(path2, y);
    fs3.reset(new FeatureStructure(spec));
  }

  {
    FeatureStructureSpec spec;
    FeaturePath path1;
    path1 += A;
    spec.content_pairs += std::make_pair(path1, z);
    fs4.reset(new FeatureStructure(spec));
  }

  {
    FeatureStructureSpec spec;
    FeaturePath path1;
    path1 += A, A;
    spec.content_pairs += std::make_pair(path1, x);
    fs5.reset(new FeatureStructure(spec));
  }

  FeatureStructureOrderer orderer;

  BOOST_CHECK(!orderer(*fs1, *fs1));
  BOOST_CHECK(orderer(*fs1, *fs2));
  BOOST_CHECK(orderer(*fs1, *fs3));
  BOOST_CHECK(orderer(*fs1, *fs4));
  BOOST_CHECK(orderer(*fs1, *fs5));

  BOOST_CHECK(!orderer(*fs2, *fs1));
  BOOST_CHECK(!orderer(*fs2, *fs2));
  BOOST_CHECK(orderer(*fs2, *fs3));
  BOOST_CHECK(orderer(*fs2, *fs4));
  BOOST_CHECK(orderer(*fs2, *fs5));

  BOOST_CHECK(!orderer(*fs3, *fs1));
  BOOST_CHECK(!orderer(*fs3, *fs2));
  BOOST_CHECK(!orderer(*fs3, *fs3));
  BOOST_CHECK(orderer(*fs3, *fs4));
  BOOST_CHECK(orderer(*fs3, *fs5));

  BOOST_CHECK(!orderer(*fs4, *fs1));
  BOOST_CHECK(!orderer(*fs4, *fs2));
  BOOST_CHECK(!orderer(*fs4, *fs3));
  BOOST_CHECK(!orderer(*fs4, *fs4));
  BOOST_CHECK(orderer(*fs4, *fs5));

  BOOST_CHECK(!orderer(*fs5, *fs1));
  BOOST_CHECK(!orderer(*fs5, *fs2));
  BOOST_CHECK(!orderer(*fs5, *fs3));
  BOOST_CHECK(!orderer(*fs5, *fs4));
  BOOST_CHECK(!orderer(*fs5, *fs5));

  typedef DereferencingOrderer<SPFS, FeatureStructureOrderer> Orderer;

  std::set<SPFS, Orderer> fs_set;

  fs_set.insert(fs1);
  BOOST_CHECK(fs_set.size() == 1);
  fs_set.insert(fs2);
  BOOST_CHECK(fs_set.size() == 2);
  fs_set.insert(fs3);
  BOOST_CHECK(fs_set.size() == 3);
  fs_set.insert(fs4);
  BOOST_CHECK(fs_set.size() == 4);
  fs_set.insert(fs5);
  BOOST_CHECK(fs_set.size() == 5);
  fs_set.insert(fs1);
  BOOST_CHECK(fs_set.size() == 5);
  fs_set.insert(fs1->Clone());
  BOOST_CHECK(fs_set.size() == 5);
}

BOOST_AUTO_TEST_CASE(TestSelectiveClone) {
  using namespace taco;
  using namespace boost::assign;

  typedef boost::shared_ptr<FeatureStructure> SPFS;

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

  SPFS fs1(new FeatureStructure(spec1));

  FeatureTree tree1;
  {
    boost::shared_ptr<FeatureTree> grand_child(new FeatureTree());
    boost::shared_ptr<FeatureTree> child(new FeatureTree());
    child->children_[C] = grand_child;
    tree1.children_[B] = child;
  }

  SPFS fs2 = fs1->SelectiveClone(tree1);

  {
    BOOST_CHECK(!fs2->IsAtomic());
    BOOST_CHECK(fs2->IsComplex());
    BOOST_CHECK(!fs2->IsEmpty());
    BOOST_CHECK(!fs2->IsEffectivelyEmpty());

    FeaturePath path1, path2, path3, path4, path5;

    path1 += A;
    path2 += A, D;
    path3 += B, C;
    path4 += B, C, D;
    path5 += B, C, E;

    SPFS v1 = fs2->Get(path1.begin(), path1.end());
    SPFS v2 = fs2->Get(path2.begin(), path2.end());
    SPFS v3 = fs2->Get(path3.begin(), path3.end());
    SPFS v4 = fs2->Get(path4.begin(), path4.end());
    SPFS v5 = fs2->Get(path5.begin(), path5.end());

    BOOST_CHECK(!v1);

    BOOST_CHECK(!v2);

    BOOST_CHECK(v3);
    BOOST_CHECK(v3->IsComplex());

    BOOST_CHECK(v4);
    BOOST_CHECK(v4->IsAtomic());
    BOOST_CHECK(v4->GetAtomicValue() == x);

    BOOST_CHECK(v5);
    BOOST_CHECK(v5->IsAtomic());
    BOOST_CHECK(v5->GetAtomicValue() == y);
  }
}

BOOST_AUTO_TEST_CASE(TestMultiClone) {
  using namespace taco;
  using namespace boost::assign;

  typedef boost::shared_ptr<FeatureStructure> SPFS;

  Vocabulary feature_set;
  Vocabulary value_set;

  const Feature A = feature_set.Insert("A");
  const Feature B = feature_set.Insert("B");
  const Feature C = feature_set.Insert("C");
  const Feature D = feature_set.Insert("D");
  const Feature E = feature_set.Insert("E");

  const AtomicValue x = value_set.Insert("x");
  const AtomicValue y = value_set.Insert("y");
  const AtomicValue z = value_set.Insert("z");
  const AtomicValue w = value_set.Insert("w");

  // Create two feature structures with unifiable values.

  FeatureStructureSpec spec1;
  {
    FeaturePath path1, path2, path3, path4;
    path1 += A;
    path2 += B, C;
    path3 += B, D;
    path4 += B, E;
    spec1.content_pairs += std::make_pair(path1, x),
                           std::make_pair(path2, y),
                           std::make_pair(path3, z),
                           std::make_pair(path4, w);
  }

  SPFS fs1(new FeatureStructure(spec1));

  FeatureStructureSpec spec2;
  {
    FeaturePath path1, path2, path3, path4;
    path1 += A;
    path2 += B, C;
    path3 += B, E;
    spec2.content_pairs += std::make_pair(path1, x),
                           std::make_pair(path2, y),
                           std::make_pair(path3, w);
  }

  SPFS fs2(new FeatureStructure(spec2));

  // First unify the feature structures' B value so that they share the
  // result.
  SPFS b1 = fs1->Get(B);
  BOOST_CHECK(b1);
  SPFS b2 = fs2->Get(B);
  BOOST_CHECK(b2);
  bool result = FeatureStructure::Unify(b1, b2);

  // Check that the resulting feature structures contain the expected values
  // and share their B value.
  {
    BOOST_CHECK(result);

    BOOST_CHECK(!fs1->IsAtomic());
    BOOST_CHECK(fs1->IsComplex());
    BOOST_CHECK(!fs1->IsEmpty());
    BOOST_CHECK(!fs1->IsEffectivelyEmpty());

    FeaturePath path1, path2, path3, path4, path5;

    path1 += A;
    path2 += B;
    path3 += B, C;
    path4 += B, D;
    path5 += B, E;

    SPFS v1 = fs1->Get(path1.begin(), path1.end());
    SPFS v2 = fs1->Get(path2.begin(), path2.end());
    SPFS v3 = fs1->Get(path3.begin(), path3.end());
    SPFS v4 = fs1->Get(path4.begin(), path4.end());
    SPFS v5 = fs1->Get(path5.begin(), path5.end());

    BOOST_CHECK(v1);
    BOOST_CHECK(v1->IsAtomic());
    BOOST_CHECK(v1->GetAtomicValue() == x);

    BOOST_CHECK(v2);
    BOOST_CHECK(v2->IsComplex());

    BOOST_CHECK(v3);
    BOOST_CHECK(v3->IsAtomic());
    BOOST_CHECK(v3->GetAtomicValue() == y);

    BOOST_CHECK(v4);
    BOOST_CHECK(v4->IsAtomic());
    BOOST_CHECK(v4->GetAtomicValue() == z);

    BOOST_CHECK(v5);
    BOOST_CHECK(v5->IsAtomic());
    BOOST_CHECK(v5->GetAtomicValue() == w);

    BOOST_CHECK(!fs2->IsAtomic());
    BOOST_CHECK(fs2->IsComplex());
    BOOST_CHECK(!fs2->IsEmpty());
    BOOST_CHECK(!fs2->IsEffectivelyEmpty());

    SPFS w1 = fs2->Get(path1.begin(), path1.end());
    SPFS w2 = fs2->Get(path2.begin(), path2.end());

    BOOST_CHECK(w1);
    BOOST_CHECK(w1 != v1);
    BOOST_CHECK(w1->IsAtomic());
    BOOST_CHECK(w1->GetAtomicValue() == x);

    BOOST_CHECK(w2);
    BOOST_CHECK(w2 == v2);
  }

  // Now perform a MultiClone to copy the feature structures.
  std::vector<SPFS> input;
  input.push_back(fs1);
  input.push_back(fs2);

  std::vector<SPFS> output(2);

  FeatureStructure::MultiClone(input.begin(), input.end(), output.begin());

  // Check the resulting feature structures.  In particular, check that their B
  // value has been cloned once and shared between them rather than being
  // cloned twice.
  {
    BOOST_CHECK(input[0] != output[0]);
    BOOST_CHECK(input[1] != output[1]);

    SPFS fs1 = output[0];
    SPFS fs2 = output[1];

    BOOST_CHECK(!fs1->IsAtomic());
    BOOST_CHECK(fs1->IsComplex());
    BOOST_CHECK(!fs1->IsEmpty());
    BOOST_CHECK(!fs1->IsEffectivelyEmpty());

    FeaturePath path1, path2, path3, path4, path5;

    path1 += A;
    path2 += B;
    path3 += B, C;
    path4 += B, D;
    path5 += B, E;

    SPFS v1 = fs1->Get(path1.begin(), path1.end());
    SPFS v2 = fs1->Get(path2.begin(), path2.end());
    SPFS v3 = fs1->Get(path3.begin(), path3.end());
    SPFS v4 = fs1->Get(path4.begin(), path4.end());
    SPFS v5 = fs1->Get(path5.begin(), path5.end());

    BOOST_CHECK(v1);
    BOOST_CHECK(v1->IsAtomic());
    BOOST_CHECK(v1->GetAtomicValue() == x);

    BOOST_CHECK(v2);
    BOOST_CHECK(v2->IsComplex());

    BOOST_CHECK(v3);
    BOOST_CHECK(v3->IsAtomic());
    BOOST_CHECK(v3->GetAtomicValue() == y);

    BOOST_CHECK(v4);
    BOOST_CHECK(v4->IsAtomic());
    BOOST_CHECK(v4->GetAtomicValue() == z);

    BOOST_CHECK(v5);
    BOOST_CHECK(v5->IsAtomic());
    BOOST_CHECK(v5->GetAtomicValue() == w);

    BOOST_CHECK(!fs2->IsAtomic());
    BOOST_CHECK(fs2->IsComplex());
    BOOST_CHECK(!fs2->IsEmpty());
    BOOST_CHECK(!fs2->IsEffectivelyEmpty());

    SPFS w1 = fs2->Get(path1.begin(), path1.end());
    SPFS w2 = fs2->Get(path2.begin(), path2.end());

    BOOST_CHECK(w1);
    BOOST_CHECK(w1 != v1);
    BOOST_CHECK(w1->IsAtomic());
    BOOST_CHECK(w1->GetAtomicValue() == x);

    BOOST_CHECK(w2);
    BOOST_CHECK(w2 == v2);
  }
}
