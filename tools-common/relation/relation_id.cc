#include "tools-common/relation/relation_id.h"

namespace taco {
namespace tool {

// TODO make this a friend of RelationId and read directly into id::value_?
std::istream &operator>>(std::istream &in, RelationId &id) {
  int i;
  in >> i;
  id = i;
  return in;
}

}  // namespace tool
}  // namespace taco
