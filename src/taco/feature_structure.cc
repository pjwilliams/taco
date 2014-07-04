#include "taco/feature_structure.h"

#include <cassert>
#include <memory>
#include <sstream>
#include <utility>

namespace taco {

FeatureStructure::FeatureStructure(const FeatureStructureSpec &spec)
    : content_(0) {
  typedef FeatureStructureSpec Spec;

  for (Spec::ContentPairSet::const_iterator p = spec.content_pairs.begin();
       p != spec.content_pairs.end(); ++p) {
    const FeaturePath &path = p->first;
    const AtomicValue &atom = p->second;
    if (path.empty()) {
      if (content_) {
        std::ostringstream msg;
        msg << "Invalid FeatureStructureSpec: multiple empty-pathed "
            << "content pairs";
        throw Exception(msg.str());
      } else if (atom == kNullAtom) {
        content_ = new internal::FSContent();
      } else {
        content_ = new internal::FSContent(atom);
      }
    } else {
      if (!content_) {
        content_ = new internal::FSContent();
      }
      CreateAtomicValue(path.begin(), path.end(), atom);
    }
  }

  for (Spec::EquivPairSet::const_iterator p = spec.equiv_pairs.begin();
       p != spec.equiv_pairs.end(); ++p) {
    const FeaturePath &path1 = p->first;
    const FeaturePath &path2 = p->second;
    boost::shared_ptr<FeatureStructure> content_ptr;
    content_ptr = GetContentBearingFS(path1.begin(), path1.end());
    if (content_ptr) {
      SetForwardPtr(path2.begin(), path2.end(), content_ptr);
    } else {
      content_ptr = GetContentBearingFS(path2.begin(), path2.end());
      assert(content_ptr);  // TODO exception?
      SetForwardPtr(path1.begin(), path1.end(), content_ptr);
    }
  }
}

FeatureStructure::~FeatureStructure() {
  delete content_;
}

boost::shared_ptr<FeatureStructure> FeatureStructure::Clone() const {
  boost::shared_ptr<FeatureStructure> clone(new FeatureStructure());
  FeatureStructure::CloneMap clone_map;
  if (forward_) {
    clone->forward_ = GetForwardTarget()->Clone(clone_map);
  } else {
    clone->content_ = content_->Clone(clone_map);
  }
  return clone;
}

boost::shared_ptr<FeatureStructure> FeatureStructure::SelectiveClone(
    const FeatureTree &tree) const {
  assert(!tree.Empty());
  boost::shared_ptr<FeatureStructure> clone(new FeatureStructure());
  FeatureStructure::CloneMap clone_map;
  if (forward_) {
    clone->forward_ = GetForwardTarget()->SelectiveClone(tree, clone_map);
  } else {
    clone->content_ = content_->SelectiveClone(tree, clone_map);
  }
  return clone;
}

// Clones the feature structure rooted at this node.  The clone_map argument
// maps a feature structure to its clone and is used to ensure that a feature
// structure is only cloned the first time it is encountered.
boost::shared_ptr<FeatureStructure> FeatureStructure::Clone(
    FeatureStructure::CloneMap &clone_map) const {
  boost::shared_ptr<FeatureStructure> clone(new FeatureStructure());
  if (forward_) {
    boost::shared_ptr<FeatureStructure> orig_fs = GetForwardTarget();
    boost::shared_ptr<FeatureStructure> &clone_fs = clone_map[orig_fs];
    if (clone_fs.get() == 0) {
      clone_fs = orig_fs->Clone(clone_map);
    }
    clone->forward_ = clone_fs;
  } else {
    clone->content_ = content_->Clone(clone_map);
  }
  return clone;
}

boost::shared_ptr<FeatureStructure> FeatureStructure::SelectiveClone(
    const FeatureTree &tree,
    FeatureStructure::CloneMap &clone_map) const {
  assert(!tree.Empty());
  boost::shared_ptr<FeatureStructure> clone(new FeatureStructure());
  if (forward_) {
    boost::shared_ptr<FeatureStructure> orig_fs = GetForwardTarget();
    boost::shared_ptr<FeatureStructure> clone_fs;
    FeatureStructure::CloneMap::iterator p = clone_map.find(orig_fs);
    if (p == clone_map.end()) {
      clone_fs = orig_fs->SelectiveClone(tree, clone_map);
      clone_map[orig_fs] = clone_fs;
    } else {
      clone_fs = p->second;
    }
    clone->forward_ = clone_fs;
  } else {
    clone->content_ = content_->SelectiveClone(tree, clone_map);
  }
  return clone;
}

bool FeatureStructure::IsEmpty() const {
  return GetContent()->Empty();
}

bool FeatureStructure::IsAtomic() const {
  return GetContent()->IsAtomic();
}

bool FeatureStructure::IsComplex() const {
  return GetContent()->IsComplex();
}

bool FeatureStructure::IsEffectivelyEmpty() const {
  return GetContent()->EffectivelyEmpty();
}

bool FeatureStructure::ContainsEmptyValue() const {
  internal::FSContent *cc = GetContent();
  if (cc->Empty() || cc->IsAtomic()) {
    return false;
  }
  for (internal::FSContent::Map::const_iterator p = cc->c.begin();
       p != cc->c.end(); ++p) {
    if (p->second->IsEmpty() || p->second->ContainsEmptyValue()) {
      return true;
    }
  }
  return false;
}

boost::shared_ptr<FeatureStructure> FeatureStructure::Get(Feature f) {
  if (forward_) {
    return GetForwardTarget()->Get(f);
  }
  internal::FSContent *cc = GetContent();
  if (cc->IsAtomic()) {
    throw Exception("get() called on atomic feature structure");
  }
  internal::FSContent::Map::iterator i = cc->c.find(f);
  if (i == cc->c.end()) {
    return boost::shared_ptr<FeatureStructure>();
  } else {
    return i->second->forward_ ? i->second->GetForwardTarget() : i->second;
  }
}

boost::shared_ptr<const FeatureStructure> FeatureStructure::Get(
    Feature f) const {
  if (forward_) {
    return GetForwardTarget()->Get(f);
  }
  internal::FSContent *cc = GetContent();
  if (cc->IsAtomic()) {
    throw Exception("get() called on atomic feature structure");
  }
  internal::FSContent::Map::iterator i = cc->c.find(f);
  if (i == cc->c.end()) {
    return boost::shared_ptr<const FeatureStructure>();
  } else {
    return i->second->forward_ ? i->second->GetForwardTarget() : i->second;
  }
}

boost::shared_ptr<FeatureStructure> FeatureStructure::Get(
    FeaturePath::const_iterator begin,
    FeaturePath::const_iterator end) {
  if (begin == end) {
    throw Exception("FeatureStructure::Get() called with empty path");
  }

  internal::FSContent *cc = GetContent();
  if (cc->IsAtomic()) {
    throw Exception("get() called on atomic feature structure");
  }

  Feature f = *begin++;
  internal::FSContent::Map::iterator i = cc->c.find(f);
  if (i == cc->c.end()) {
    return boost::shared_ptr<FeatureStructure>();
  } else if (begin == end) {
    return i->second->forward_ ? i->second->GetForwardTarget() : i->second;
  } else {
    return i->second->Get(begin, end);
  }
}

boost::shared_ptr<const FeatureStructure> FeatureStructure::Get(
    FeaturePath::const_iterator begin,
    FeaturePath::const_iterator end) const {
  if (begin == end) {
    throw Exception("FeatureStructure::Get() called with empty path");
  }

  internal::FSContent *cc = GetContent();
  if (cc->IsAtomic()) {
    throw Exception("FeatureStructure::Get() called on atomic feature structure");
  }

  Feature f = *begin++;
  internal::FSContent::Map::iterator i = cc->c.find(f);
  if (i == cc->c.end()) {
    return boost::shared_ptr<const FeatureStructure>();
  } else if (begin == end) {
    return i->second->forward_ ? i->second->GetForwardTarget() : i->second;
  } else {
    return i->second->Get(begin, end);
  }
}

AtomicValue FeatureStructure::GetAtomicValue() const {
  internal::FSContent *p = GetContent();
  if (p->IsComplex()) {
    throw Exception("GetAtomicValue() called on complex feature structure");
  }
  return p->a;
}

void FeatureStructure::Redirect(boost::shared_ptr<FeatureStructure> &other) {
  if (forward_) {
    assert(content_ == 0);
    Dechain();
    delete forward_->content_;
    forward_->content_ = 0;
    forward_->forward_ = other;
  } else {
    delete content_;
    content_ = 0;
  }
  forward_ = other;
}

bool FeatureStructure::Unify(boost::shared_ptr<FeatureStructure> &lhs,
                             boost::shared_ptr<FeatureStructure> &rhs) {
  // Dereference to get direct pointers to content objects.
  internal::FSContent *lhs_content = lhs->GetContent();
  internal::FSContent *rhs_content = rhs->GetContent();

  // If the feature structures are already unified then there is nothing to do.
  if (lhs_content == rhs_content) {
    return true;
  }

  // Check for the case that one or both values are empty.
  if (lhs_content->Empty()) {
    lhs->Redirect(rhs);
    return true;
  } else if (rhs_content->Empty()) {
    rhs->Redirect(lhs);
    return true;
  }

  // Atomic case.
  if (lhs_content->IsAtomic()) {
    if (rhs_content->IsComplex() || lhs_content->a != rhs_content->a) {
      // Note: The first condition in the above if statement is redundant
      // because if lhs_content is atomic and rhs_content is complex then they
      // must have different values of a.  But if the implementation of
      // FSContent changes then that may cease to be the case, so the check
      // has been left in defensively.
      return false;
    }
    lhs->Redirect(rhs);
    return true;
  } else if (rhs_content->IsAtomic()) {
    return false;
  }

  // Complex case.

  // TODO Can this be optimised based on the sizes of the FSs?  We want to
  // minimise the number of insertions into the flat_map.
  internal::FSContent::Map::iterator p;
  internal::FSContent::Map::iterator end = lhs_content->c.end();
  for (p = lhs_content->c.begin(); p != end; ++p) {
    Feature f = p->first;
    boost::shared_ptr<FeatureStructure> &lhs_fs = p->second;
    boost::shared_ptr<FeatureStructure> &rhs_fs = rhs_content->c[f];
    if (rhs_fs.get() == 0) {  // Didn't previously exist.
      // FIXME Is this OK?  Or should we create an empty FS then unify?  (I
      // think it's OK since the feature structures are acyclic.)
      rhs_fs = lhs_fs;
    } else if (!Unify(lhs_fs, rhs_fs)) {
      return false;
    }
  }

  // FIXME Should lhs->forward_ be adjusted before the for loop?  (I
  // think it's OK since the feature structures are acyclic: nothing 'inside'
  // lhs_content or rhs_content can .)
  lhs->Redirect(rhs);

  return true;
}

bool FeatureStructure::PossiblyUnifiable(
    boost::shared_ptr<const FeatureStructure> other) const {
  const internal::FSContent *lhs_content = GetContent();
  const internal::FSContent *rhs_content = other->GetContent();

  // If the feature structures are already unified then there is nothing to do.
  if (lhs_content == rhs_content) {
    return true;
  }

  // Check for the case that one or both values are empty.
  if (lhs_content->Empty() || rhs_content->Empty()) {
    return true;
  }

  // Atomic case.
  if (lhs_content->IsAtomic()) {
    if (rhs_content->IsComplex() || lhs_content->a != rhs_content->a) {
      return false;
    }
    return true;
  } else if (rhs_content->IsAtomic()) {
    return false;
  }

  // Complex case.

  internal::FSContent::Map::const_iterator p;
  internal::FSContent::Map::const_iterator end = lhs_content->c.end();
  for (p = lhs_content->c.begin(); p != end; ++p) {
    Feature f = p->first;
    boost::shared_ptr<const FeatureStructure> lhs_fs = p->second;
    internal::FSContent::Map::const_iterator q = rhs_content->c.find(f);
    if (q == rhs_content->c.end()) {
      // Unifiable: rhs_content->c[f] would be set to lhs_fs
      continue;
    } else {
      const boost::shared_ptr<FeatureStructure> &rhs_fs = q->second;
      if (!rhs_fs->PossiblyUnifiable(lhs_fs)) {
        return false;
      }
    }
  }

  return true;
}

boost::shared_ptr<FeatureStructure> FeatureStructure::GetForwardTarget() const {
  if (forward_) {
    Dechain();
    return forward_;
  }
  // TODO Just return forward_?
  return boost::shared_ptr<FeatureStructure>();
}

internal::FSContent *FeatureStructure::GetContent() const {
  if (forward_) {
    Dechain();
    return forward_->content_;
  }
  return content_;
}

void FeatureStructure::Dechain() const {
  if (!forward_) {
    return;
  }
  while (forward_->forward_) {
    forward_ = forward_->forward_;
  }
}

// TODO Don't allow atom to be kNullAtom (CreateEmptyValue() currently uses
// this, so requires dependency to be removed).
boost::shared_ptr<FeatureStructure> FeatureStructure::CreateAtomicValue(
    FeaturePath::const_iterator begin,
    FeaturePath::const_iterator end,
    AtomicValue atom) {

  if (content_->IsAtomic()) {
    std::ostringstream msg;
    msg << "FeatureStructure::CreateAtomicValue() called on atomic feature "
        << "structure";
    throw Exception(msg.str());
  }

  if (begin == end) {
    std::string msg;
    msg = "FeatureStructure::CreateAtomicValue() called with empty path";
    throw Exception(msg);
  }

  Feature f = *begin++;

  if (begin == end) {
    // f is the last feature in the path.  Insert the value into this
    // feature structure's content object.
    boost::shared_ptr<FeatureStructure> value(new FeatureStructure());
    if (atom == kNullAtom) {
      value->content_ = new internal::FSContent();
    } else {
      value->content_ = new internal::FSContent(atom);
    }
    std::pair<internal::FSContent::Map::iterator, bool> result;
    result = content_->c.insert(std::make_pair(f, value));
    // TODO Exception?
    assert(result.second);
    return value;
  }

  // Recursive step.
  boost::shared_ptr<FeatureStructure> inner;
  internal::FSContent::Map::iterator i = content_->c.find(f);
  // TODO Fix double lookup
  if (i == content_->c.end()) {
    inner.reset(new FeatureStructure());
    inner->content_ = new internal::FSContent();
    content_->c[f] = inner;
  } else {
    inner = i->second;
    if (inner->IsAtomic()) {
      // TODO Exception?
      assert(false);
    }
  }
  return inner->CreateAtomicValue(begin, end, atom);
}

// TODO
boost::shared_ptr<FeatureStructure> FeatureStructure::CreateEmptyValue(
    FeaturePath::const_iterator begin, FeaturePath::const_iterator end) {
  return CreateAtomicValue(begin, end, kNullAtom);
}

void FeatureStructure::SetForwardPtr(
    FeaturePath::const_iterator begin,
    FeaturePath::const_iterator end,
    boost::shared_ptr<FeatureStructure> fwd_ptr) {
  assert(begin != end);

  Feature f = *begin++;

  internal::FSContent *cc = GetContent();
  assert(cc && cc->IsComplex());

  FeatureStructure *inner;
  // TODO Fix double lookup
  internal::FSContent::Map::iterator p = cc->c.find(f);
  if (p != cc->c.end()) {
    inner = p->second.get();
  } else {
    inner = new FeatureStructure();
    cc->c[f] = boost::shared_ptr<FeatureStructure>(inner);
    if (begin == end) {
      inner->forward_ = fwd_ptr;
      return;
    }
    inner->content_ = new internal::FSContent();
  }
  inner->SetForwardPtr(begin, end, fwd_ptr);
}

boost::shared_ptr<FeatureStructure> FeatureStructure::GetContentBearingFS(
    FeaturePath::const_iterator begin,
    FeaturePath::const_iterator end) {
  assert(begin != end);

  internal::FSContent *cc = GetContent();
  assert(cc && cc->IsComplex());

  Feature f = *begin++;
  internal::FSContent::Map::iterator p = cc->c.find(f);
  if (p == cc->c.end()) {
    return boost::shared_ptr<FeatureStructure>();
  } else if (begin == end) {
    return p->second->forward_ ? p->second->GetForwardTarget()
                               : p->second;
  } else {
    return p->second->GetContentBearingFS(begin, end);
  }
}

namespace internal {

bool FSContent::EffectivelyEmpty() const {
  if (a != kNullAtom) {
    return false;
  }
  for (Map::const_iterator p = c.begin(); p != c.end(); ++p) {
    boost::shared_ptr<FeatureStructure> value = p->second;
    if (!value->IsEffectivelyEmpty()) {
      return false;
    }
  }
  return true;
}

FSContent *FSContent::Clone(FeatureStructure::CloneMap &clone_map) const {
  typedef FeatureStructure::CloneMap CloneMap;
  if (a != kNullAtom) {
    return new FSContent(a);
  }
  FSContent *clone = new FSContent();
  clone->c.reserve(c.size());
  for (Map::const_iterator p = c.begin(); p != c.end(); ++p) {
    Feature f = p->first;
    boost::shared_ptr<FeatureStructure> orig_fs = p->second;
    CloneMap::value_type x(orig_fs, boost::shared_ptr<FeatureStructure>());
    std::pair<CloneMap::iterator, bool> result = clone_map.insert(x);
    if (result.second) {  // orig_fs was not already in clone_map
      result.first->second = orig_fs->Clone(clone_map);
    }
    clone->c.insert(std::make_pair(f, result.first->second));
  }
  return clone;
}

// TODO Rewrite this to avoid map lookup
FSContent *FSContent::SelectiveClone(
    const FeatureTree &tree,
    FeatureStructure::CloneMap &clone_map) const {
  assert(!tree.Empty());
  if (a != kNullAtom) {
    return new FSContent(a);
  }
  FSContent *clone = new FSContent();
  for (Map::const_iterator p = c.begin(); p != c.end(); ++p) {
    Feature f = p->first;
    // TODO Shouldn't be doing a map lookup here.  Should be jointly iterating
    // over tree.children_ and c.
    FeatureTree::ChildMap::const_iterator child_iter = tree.children_.find(f);
    if (child_iter == tree.children_.end()) {
      continue;
    }
    const FeatureTree &sub_tree = *(child_iter->second);
    boost::shared_ptr<FeatureStructure> orig_fs = p->second;
    boost::shared_ptr<FeatureStructure> clone_fs;
    FeatureStructure::CloneMap::iterator q = clone_map.find(orig_fs);
    if (q == clone_map.end()) {
      if (sub_tree.Empty()) {
        clone_fs = orig_fs->Clone(clone_map);
      } else {
        clone_fs = orig_fs->SelectiveClone(sub_tree, clone_map);
      }
      clone_map[orig_fs] = clone_fs;
    } else {
      clone_fs = q->second;
    }
    clone->c.insert(std::make_pair(f, clone_fs));
  }
  return clone;
}

bool ComplexContentValueOrderer::operator()(
  const FSContent::Map::value_type &a,
  const FSContent::Map::value_type &b) const {
  if (a.first == b.first) {
    FeatureStructureOrderer orderer;
    return orderer(*a.second, *b.second);
  }
  return a.first < b.first;
}

bool ComplexContentOrderer::operator()(const FSContent &a,
                                       const FSContent &b) const {
  return std::lexicographical_compare(a.c.begin(), a.c.end(),
                                      b.c.begin(), b.c.end(),
                                      ComplexContentValueOrderer());
}

}  // namespace internal

// Defines a completely arbitrary strict weak ordering between FeatureStructures
// so they can be stored in std::sets and suchlike.  Compares atomic values
// only: *does not* distinguish between structure sharing differences.
bool FeatureStructureOrderer::operator()(const FeatureStructure &a,
                                         const FeatureStructure &b) const {
  if (a.IsAtomic()) {
    if (!b.IsAtomic()) {
      return true;
    }
    return a.GetAtomicValue() < b.GetAtomicValue();
  } else if (b.IsAtomic()) {
    return false;
  }
  internal::ComplexContentOrderer orderer;
  return orderer(*(a.GetContent()), *(b.GetContent()));
}

}  // namespace taco
