#ifndef TACO_TOOLS_COMMON_TEXT_FORMATS_CONSTRAINT_EXTRACT_WRITER_H_
#define TACO_TOOLS_COMMON_TEXT_FORMATS_CONSTRAINT_EXTRACT_WRITER_H_

#include "taco/constraint_set.h"
#include "taco/base/string_piece.h"
#include "taco/text-formats/constraint_writer.h"

#include <ostream>
#include <string>
#include <vector>

namespace taco {
namespace tool {

class ConstraintExtractWriter {
 public:
  ConstraintExtractWriter(const ConstraintWriter &constraint_writer,
                          std::ostream &output)
      : constraint_writer_(constraint_writer)
      , output_(output) {}

  // Write a single line of a constraint extract file given the rule's
  // target LHS, target RHS, and a sequence of constraint sets (InputIterator).
  // The constraint set sequence is allowed to be empty but the constraint sets
  // must be non-empty.  Throws a taco::Exception if this precondition is not
  // satisfied.
  template<typename InputIterator>
  void Write(const std::string &, const std::vector<std::string> &,
             InputIterator, InputIterator) const;

 private:
  const ConstraintWriter &constraint_writer_;
  std::ostream &output_;
};

template<typename InputIterator>
void ConstraintExtractWriter::Write(const std::string &lhs,
                                    const std::vector<std::string> &rhs,
                                    InputIterator first_cs,
                                    InputIterator last_cs) const {
  // Output the target-side LHS and RHS.
  output_ << lhs << " |||";
  for (std::vector<std::string>::const_iterator p = rhs.begin();
       p != rhs.end(); ++p) {
    output_ << " " << *p;
  }

  // Output the sequence of constraint sets.
  output_ << " |||";
  bool first = true;
  while (first_cs != last_cs) {
    const ConstraintSet &cs = **first_cs++;
    if (cs.IsEmpty()) {
      throw Exception("empty constraint set");
    }
    if (!first) {
      output_ << " :";
    } else {
      first = false;
    }
    for (AbsConstraintSet::ConstIterator q = cs.abs_set().Begin();
         q != cs.abs_set().End(); ++q) {
      output_ << " ";
      constraint_writer_.WriteAbsConstraint(**q, output_);
    }
    for (RelConstraintSet::ConstIterator q = cs.rel_set().Begin();
         q != cs.rel_set().End(); ++q) {
      output_ << " ";
      constraint_writer_.WriteRelConstraint(**q, output_);
    }
    for (VarConstraintSet::ConstIterator q = cs.var_set().Begin();
         q != cs.var_set().End(); ++q) {
      output_ << " ";
      constraint_writer_.WriteVarConstraint(**q, output_);
    }
  }

  output_ << std::endl;
}

}  // namespace tool
}  // namespace taco

#endif
