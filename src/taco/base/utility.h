#ifndef TACO_SRC_TACO_BASE_UTILITY_H_
#define TACO_SRC_TACO_BASE_UTILITY_H_

namespace taco {

// TODO There *must* be a standard way of doing this, but I can't think of it
// at the moment.
// TODO Rename to DereferencingBinaryPred
template<typename PointerType, typename Orderer>
class DereferencingOrderer {
 public:
  bool operator()(PointerType a, PointerType b) const {
    return orderer_(*a, *b);
  }
 private:
  Orderer orderer_;
};

}  // namespace taco

#endif
