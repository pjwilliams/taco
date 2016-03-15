#ifndef TACO_SRC_TACO_BASE_HASH_COMBINE_H_
#define TACO_SRC_TACO_BASE_HASH_COMBINE_H_

#include <cstddef>

namespace taco {

// This is Boost's hash_combine function except that it takes a hash value
// as the second argument instead of calling hash_value().
inline void hash_combine(std::size_t &seed, std::size_t hash_value) {
  seed ^= hash_value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

}  // namespace taco

#endif
