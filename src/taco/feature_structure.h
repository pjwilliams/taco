#ifndef TACO_SRC_TACO_FEATURE_STRUCTURE_H_
#define TACO_SRC_TACO_FEATURE_STRUCTURE_H_

#include <map>

#include <boost/container/flat_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "taco/base/basic_types.h"
#include "taco/base/exception.h"
#include "taco/feature_path.h"
#include "taco/feature_structure_spec.h"
#include "taco/feature_tree.h"

namespace taco {

namespace internal {
struct FSContent;
}

// Represents an untyped, acyclic feature structure as in the PATR-II
// formalism (Shieber, 1986).  Feature structures are represented as nodes
// in a directed acyclic graph (DAG) that is rooted and has labelled edges.
// Every feature structure either owns 'content' -- an atomic value or a mapping
// from features to values -- or can access a feature structure with content
// via a sequence of one or more forward pointers (reentrancy).  Semantically,
// there is only one level of indirection, but chaining of forward pointers can
// occur can as a natural result of the unification algorithm.  Chains will be
// eliminated as early as possible.
// TODO note about use of shared_ptr
// TODO *might* represent atomic and complex feature structures as separate
// classes since most operations only make sense for one type.
class FeatureStructure {
 public:
  FeatureStructure(const FeatureStructureSpec &);

  ~FeatureStructure();

  // Produces deep copies of the FeatureStructures in the range [first,last),
  // assigning them to a range beginning at result.  A MultiClone preserves
  // reentrancy between feature structures instead of creating one copy of a
  // shared value for each feature structure, as would occur with a sequence of
  // Clone() operations.
  template<typename InputIterator, typename OutputIterator>
  static OutputIterator MultiClone(InputIterator first, InputIterator last,
                                   OutputIterator result);

  // Produces a deep copy of this FeatureStructure.  The clone will have
  // identical, and identically-coindexed, values, though will be completely
  // independent of the original.
  boost::shared_ptr<FeatureStructure> Clone() const;

  // Produces a partial deep-copy of this FeatureStructure.  A value is cloned
  // iff its feature path is present in the given FeatureTree.  The FeatureTree
  // must be non-empty.
  boost::shared_ptr<FeatureStructure> SelectiveClone(const FeatureTree &) const;
 
  bool IsAtomic() const;
  bool IsComplex() const;
  bool IsEmpty() const;

  // Returns true if this FeatureStructure is either empty or is complex and
  // all of its values are effectively empty.
  bool IsEffectivelyEmpty() const;

  // Returns false if this FeatureStructure is empty or atomic.  Otherwise,
  // recursively checks the values of its features looking for an empty value,
  // returning true iff one is found.
  bool ContainsEmptyValue() const;

  // Gets the value associated with the given feature, if one exists, or
  // an empty pointer otherwise.
  boost::shared_ptr<FeatureStructure> Get(Feature);
  boost::shared_ptr<const FeatureStructure> Get(Feature) const;

  // Gets the value corresponding to the given path, if valid, or an empty
  // pointer otherwise.  The path must be non-empty as there is no way for
  // this feature structure to obtain a valid shared_ptr to itself.
  boost::shared_ptr<FeatureStructure> Get(
    FeaturePath::const_iterator begin,
    FeaturePath::const_iterator end);
  boost::shared_ptr<const FeatureStructure> Get(
    FeaturePath::const_iterator begin,
    FeaturePath::const_iterator end) const;

  // Return the atomic value associated with this feature structure.
  AtomicValue GetAtomicValue() const;

  // Creates an atomic feature structure at the path specified by the
  // iterators.  Any enclosing feature structures are created as necessary.
  // The atomic feature structure is required not to exist and the path is
  // required to be non-empty.
  boost::shared_ptr<FeatureStructure> CreateAtomicValue(
    FeaturePath::const_iterator begin,
    FeaturePath::const_iterator end,
    AtomicValue);

  // Creates an empty feature structure at the path specified by the
  // iterators.  Any enclosing feature structures are created as necessary.
  // The empty feature structure is required not to exist and the path is
  // required to be non-empty.
  boost::shared_ptr<FeatureStructure> CreateEmptyValue(
    FeaturePath::const_iterator begin,
    FeaturePath::const_iterator end);

  // Attempts to unify two feature structures.  Returns true on success.
  // Unification is destructive with the result that after unifcation the two
  // feature structures will have been merged.  Both will therefore share the
  // same value (available to both via forwarding).  If unification fails, the
  // values of the two feature structures will be undefined, however they will
  // not leak: on their destruction all owned content will be destroyed as
  // usual.
  // TODO That's the intention anyway.  Review the code to double-check this is
  // really true.
  static bool Unify(boost::shared_ptr<FeatureStructure> &,
                    boost::shared_ptr<FeatureStructure> &);

  // Performs a cheap, non-destructive test to determine if unficiation between
  // this feature structure and another might succeed.  The test can produce
  // false positives, but not false negatives.
  bool PossiblyUnifiable(boost::shared_ptr<const FeatureStructure>) const;

  //bool subsumes(const FeatureStructure &) const;

 private:
  friend class FeatureStructureOrderer;
  friend struct internal::FSContent;

  typedef boost::unordered_map<boost::shared_ptr<FeatureStructure>,
                               boost::shared_ptr<FeatureStructure> > CloneMap;

  FeatureStructure() : content_(0) {}

  // Copying is not allowed
  FeatureStructure(const FeatureStructure &);
  FeatureStructure &operator=(const FeatureStructure &);

  // Implements the public clone() function.
  boost::shared_ptr<FeatureStructure> Clone(CloneMap &) const;

  // Implements the public selectiveClone() function.
  boost::shared_ptr<FeatureStructure> SelectiveClone(const FeatureTree &,
                                                     CloneMap &) const;

  // Returns a pointer to the content-bearing feature structure at the end of
  // the chain of forward pointers.  Returns an empty pointer if this is
  // not a forwarding feature structure.
  boost::shared_ptr<FeatureStructure> GetForwardTarget() const;

  // Gets a pointer to the FSContent object owned by either this feature
  // structure or the feature structure at the end of the forwarding
  // chain.
  internal::FSContent *GetContent() const;

  // Follows the chain of forward pointers and replaces forward_ with a
  // direct pointer to the content-bearing feature structure.  Has no effect
  // if this is content-bearing feature structure.
  void Dechain() const;

  // Sets the forward pointer of the feature structure at the path
  // specified by the iterators.  Any intervening feature structures are
  // created as necessary.  The final feature structure will be created and
  // is required not to already exist.
  void SetForwardPtr(FeaturePath::const_iterator begin,
                     FeaturePath::const_iterator end,
                     boost::shared_ptr<FeatureStructure>);

  // Gets a shared pointer to the content-bearing feature structure at the path
  // specified by the iterators.  This requires that we can locate the preceding
  // feature structure, so the path must be non-empty.  i.e. We can't get
  // the shared pointer that points to *this* feature structure.
  boost::shared_ptr<FeatureStructure> GetContentBearingFS(
    FeaturePath::const_iterator begin,
    FeaturePath::const_iterator end);

  // Modifies this feature structure so that it forwards to the given FS.
  // If this FS already forwards to another FS then the latter is also
  // updated.
  void Redirect(boost::shared_ptr<FeatureStructure> &);

  mutable boost::shared_ptr<FeatureStructure> forward_;
  internal::FSContent *content_;
};

class FeatureStructureOrderer {
 public:
  bool operator()(const FeatureStructure &, const FeatureStructure &) const;
};

template<typename InputIterator, typename OutputIterator>
OutputIterator FeatureStructure::MultiClone(InputIterator first,
                                            InputIterator last,
                                            OutputIterator result) {
  CloneMap clone_map;
  while (first != last) {
    *result = (*first)->Clone(clone_map);
    ++first;
    ++result;
  }
  return result;
}

namespace internal {

struct FSContent {
  typedef boost::container::flat_map<Feature,
                                     boost::shared_ptr<FeatureStructure> > Map;
  FSContent() : a(kNullAtom) {}
  FSContent(AtomicValue x) : a(x) {}
  bool IsAtomic() const { return a != kNullAtom; }
  bool IsComplex() const { return a == kNullAtom; }
  bool Empty() const { return a == kNullAtom ? c.empty() : false; }
  bool EffectivelyEmpty() const;
  FSContent *Clone(FeatureStructure::CloneMap &) const ;
  FSContent *SelectiveClone(const FeatureTree &,
                            FeatureStructure::CloneMap &) const;
  Map c;
  AtomicValue a;
};

class ComplexContentValueOrderer {
 public:
  bool operator()(const FSContent::Map::value_type &,
                  const FSContent::Map::value_type &) const;
};

class ComplexContentOrderer {
 public:
  bool operator()(const FSContent &, const FSContent &) const;
};

}  // namespace internal

}  // namespace taco

#endif
