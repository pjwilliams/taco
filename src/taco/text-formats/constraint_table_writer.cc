#include "taco/text-formats/constraint_table_writer.h"

#include "taco/constraint_set.h"

namespace taco {

void ConstraintTableWriter::Write(unsigned int id,
                                  const ConstraintSet &cs) const {
  output_ << id << " |||";
  // Write AbsConstraints first.
  for (AbsConstraintSet::ConstIterator p = cs.abs_set().Begin();
       p != cs.abs_set().End(); ++p) {
    output_ << " ";
    constraint_writer_.WriteAbsConstraint(**p, output_);
  }
  // Then write RelConstraints.
  for (RelConstraintSet::ConstIterator p = cs.rel_set().Begin();
       p != cs.rel_set().End(); ++p) {
    output_ << " ";
    constraint_writer_.WriteRelConstraint(**p, output_);
  }
  // Finally, write VarConstraints.
  for (VarConstraintSet::ConstIterator p = cs.var_set().Begin();
       p != cs.var_set().End(); ++p) {
    output_ << " ";
    constraint_writer_.WriteVarConstraint(**p, output_);
  }
  output_ << std::endl;
}

}  // namespace taco
