#ifndef TACO_TOOLS_COMMON_RELATION_ID_H_
#define TACO_TOOLS_COMMON_RELATION_ID_H_

#include <istream>

namespace taco {
namespace tool {

class RelationId {
 public:
  RelationId() : value_(-1) {}
  explicit RelationId(int v) : value_(v) {}
  RelationId &operator=(int i) { value_ = i; return *this; }

  // FIXME
  //operator int() const { return value_; }
  int value() const { return value_; }

 private:
  int value_;
};

std::istream &operator>>(std::istream &, RelationId &);

}  // namespace tool
}  // namespace taco

#endif
