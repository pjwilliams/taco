#ifndef TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_WRITER_H_
#define TACO_SRC_TACO_TEXT_FORMATS_CONSTRAINT_WRITER_H_

#include <ostream>

#include "taco/base/vocabulary.h"
#include "taco/constraint.h"

namespace taco {

class ConstraintWriter {
 public:
  ConstraintWriter(const Vocabulary &feature_set, const Vocabulary &value_set)
      : feature_set_(feature_set)
      , value_set_(value_set) {}

  void WriteAbsConstraint(const AbsConstraint &, std::ostream &) const;
  void WriteRelConstraint(const RelConstraint &, std::ostream &) const;
  void WriteVarConstraint(const VarConstraint &, std::ostream &) const;

  void WritePathTerm(const PathTerm &, std::ostream &) const;
  void WriteValueTerm(const ValueTerm &, std::ostream &) const;
  void WriteVarTerm(const VarTerm &, std::ostream &) const;

 private:
  const Vocabulary &feature_set_;
  const Vocabulary &value_set_;
};

}  // namespace taco

#endif
