#ifndef TACO_SRC_TACO_BASE_BASIC_TYPES_H_
#define TACO_SRC_TACO_BASE_BASIC_TYPES_H_

#include <boost/cstdint.hpp>
#include <boost/integer_traits.hpp>

namespace taco {

typedef boost::uint_least32_t AtomicValue;
typedef boost::uint_least32_t Feature;

const AtomicValue kNullAtom = boost::integer_traits<AtomicValue>::const_max;

}  // namespace taco

#endif
