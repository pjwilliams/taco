#ifndef TACO_SRC_TACO_BASE_VOCABULARY_H_
#define TACO_SRC_TACO_BASE_VOCABULARY_H_

#include <string>

#include "taco/base/basic_types.h"
#include "taco/base/numbered_set.h"

namespace taco {

typedef NumberedSet<std::string, AtomicValue> Vocabulary;

}  // namespace taco

#endif
