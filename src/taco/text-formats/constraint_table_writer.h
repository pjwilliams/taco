#ifndef TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_TABLE_WRITER_H_
#define TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_TABLE_WRITER_H_

#include <ostream>

#include "taco/text-formats/constraint_writer.h"

namespace taco {

class ConstraintSet;

class ConstraintTableWriter {
 public:
  ConstraintTableWriter(const ConstraintWriter &constraint_writer,
                        std::ostream &output)
      : constraint_writer_(constraint_writer)
      , output_(output) {}

  void Write(unsigned int, const ConstraintSet &) const;

 private:
  const ConstraintWriter &constraint_writer_;
  std::ostream &output_;
};

}  // namespace taco

#endif
