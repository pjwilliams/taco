#ifndef TACO_TOOLS_COMMON_RELATION_TREE_OPS_H_
#define TACO_TOOLS_COMMON_RELATION_TREE_OPS_H_

#include "relation.h"
#include "relation_id.h"

#include <boost/tuple/tuple.hpp>
#include <boost/type_traits.hpp>

#include <istream>
#include <map>
#include <set>
#include <vector>

namespace taco {
namespace tool {

namespace internal {

// TODO There *must* be a way to do this in standard C++ or Boost.
template<typename T>
class TypeConvert {
 public:
  typedef typename boost::remove_pointer<T>::type DataType;
  static const DataType &dereference_if_pointer(const T &value) {
    return value;
  }
};

template<typename T>
class TypeConvert<T*> {
 public:
  typedef T DataType;
  typedef T* PointerType;
  static const DataType &dereference_if_pointer(const PointerType &value) {
    return *value;
  }
};

}  // namespace internal

// Given a tuple tree with relation IDs, sets all IDs to -1.
//
// TreeType   A tuple-based SyntaxTree type
// N          Index of relation ID in tuple
//
template<typename TreeType, int N>
void ClearIds(TreeType &t) {
  boost::tuples::get<N>(t.label()) = -1;
  const std::vector<TreeType *> &children = t.children();
  for (typename std::vector<TreeType *>::const_iterator p = children.begin();
       p != children.end(); ++p) {
    TreeType &child = **p;
    ClearIds<TreeType,N>(child);
  }
}

// Given a tuple tree with relation IDs, populates a map from IDs to Relations.
//
// TreeType         A tuple-based SyntaxTree type
// N                Index of relation ID in tuple
// RelationMapType  Map from RelationId to Relation<const TreeType*>
//
template<typename TreeType, int N, typename RelationMapType>
void ExtractRelations(const TreeType &t, RelationMapType &relation_map) {
  RelationId id = boost::tuples::get<N>(t.label());
  if (id.value() != -1) {
    relation_map[id.value()].nodes.insert(&t);
  }
  const std::vector<TreeType *> &children = t.children();
  for (typename std::vector<TreeType *>::const_iterator p = children.begin();
       p != children.end(); ++p) {
    const TreeType &child = **p;
    ExtractRelations<TreeType, N, RelationMapType>(child, relation_map);
  }
}

// Non-const TreeType version of above.
// RelationMapType should be Relation<TreeType*>
// TODO Common implementation?
template<typename TreeType, int N, typename RelationMapType>
void ExtractRelations(TreeType &t, RelationMapType &relation_map) {
  RelationId id = boost::tuples::get<N>(t.label());
  if (id.value() != -1) {
    relation_map[id.value()].nodes.insert(&t);
  }
  std::vector<TreeType *> &children = t.children();
  for (typename std::vector<TreeType *>::const_iterator p = children.begin();
       p != children.end(); ++p) {
    TreeType &child = **p;
    ExtractRelations<TreeType, N, RelationMapType>(child, relation_map);
  }
}

// Given a tuple tree and a set of relations defined over its nodes, allocates
// a number to each relation and populates a map from numbers to relations.
// The number allocated is the lowest index (zero-based) of any leaf belonging
// to the relation.
//
// TreeType         A tuple-based SyntaxTree type
// RelationSetType  Set of RelationType
// RelationMapType  Map from int to const RelationType *
//
template<typename TreeType, typename RelationSetType, typename RelationMapType>
void NumberRelations(const TreeType &t,
                     const RelationSetType &relations,
                     RelationMapType &relation_map) {
  typedef std::map<const TreeType *, int> LeafIndexMap;
  typedef typename RelationSetType::value_type RelationType;

  relation_map.clear();

  std::vector<const TreeType *> leaves;
  t.GetLeaves(leaves);

  LeafIndexMap leaf_index_map;
  int pos = 0;
  for (typename std::vector<const TreeType *>::const_iterator p(leaves.begin());
       p != leaves.end(); ++p) {
    leaf_index_map[*p] = pos++;
  }

  for (typename RelationSetType::const_iterator p(relations.begin());
       p != relations.end(); ++p) {
    const RelationType &relation = *p;
    int min_index = -1;
    for (typename RelationType::ConstIterator q = relation.Begin();
         q != relation.End(); ++q) {
      const TreeType *tree = *q;
      if (tree->IsLeaf()) {
        int index = leaf_index_map[tree];
        if (min_index == -1 || index < min_index) {
          min_index = index;
        }
      }
    }
    relation_map[min_index] = &relation;
  }
}

// Given a tuple tree with relation IDs and a map from relation IDs to
// relations, sets the relation ID of every node to the ID of the relation
// to which it belongs (or -1 if none).
//
// TreeType             A tuple-based SyntaxTree type
// N                    Index of relation ID in tuple
// MappedRelationType   Map from RelationId to Relation<const TreeType*>
//
// TODO Template on MapType and use MapType::mapped_type instead of templating
// on MappedRelationType
template<typename TreeType, int N, typename MappedRelationType>
void SetRelationIds(TreeType &t,
                    const std::map<int, MappedRelationType> &relation_map) {
  typedef Relation<TreeType *> RelationType;
  typedef std::map<int, MappedRelationType> MapType;
  ClearIds<TreeType,N>(t);
  for (typename MapType::const_iterator p = relation_map.begin();
       p != relation_map.end(); ++p) {
    int id = p->first;
    const MappedRelationType &mapped_relation = p->second;
    const RelationType &relation = internal::TypeConvert<MappedRelationType>::dereference_if_pointer(mapped_relation);
    for (typename RelationType::ConstIterator q = relation.Begin();
         q != relation.End(); ++q) {
      boost::tuples::get<N>((*q)->label()) = id;
    }
  }
}

}  // namespace tool
}  // namespace taco

#endif
