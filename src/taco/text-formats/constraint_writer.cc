#include "taco/text-formats/constraint_writer.h"

#include <iomanip>

namespace taco {

void ConstraintWriter::WriteAbsConstraint(const AbsConstraint &constraint,
                                          std::ostream &out) const {
  WritePathTerm(constraint.lhs, out);
  out << "=";
  WriteValueTerm(constraint.rhs, out);
}

void ConstraintWriter::WriteRelConstraint(const RelConstraint &constraint,
                                          std::ostream &out) const {
  WritePathTerm(constraint.lhs, out);
  out << "=";
  WritePathTerm(constraint.rhs, out);
}

void ConstraintWriter::WriteVarConstraint(const VarConstraint &constraint,
                                          std::ostream &out) const {
  WritePathTerm(constraint.lhs, out);
  out << "=";
  WriteVarTerm(constraint.rhs, out);
}

void ConstraintWriter::WritePathTerm(const PathTerm &term,
                                     std::ostream &out) const {
  out << "<" << term.index();
  for (FeaturePath::const_iterator p = term.path().begin();
       p != term.path().end(); ++p) {
    out << "\"" << feature_set_.Lookup(*p) << "\"";
  }
  out << ">";
}

void ConstraintWriter::WriteValueTerm(const ValueTerm &term,
                                      std::ostream &out) const {
  out << "\"" << value_set_.Lookup(term.value()) << "\"";
}

void ConstraintWriter::WriteVarTerm(const VarTerm &term,
                                    std::ostream &out) const {
  out << "{";
  for (VarTerm::ProbabilityMap::const_iterator p = term.probabilities().begin();
       p != term.probabilities().end(); ++p) {
    AtomicValue val = p->first;
    float prob = p->second;
    if (p != term.probabilities().begin()) {
      out << ",";
    }
    out << "\"" << value_set_.Lookup(val) << "\":"
        << std::fixed << std::setprecision(3) << prob;
  }
  out << "}";
}

}  // namespace taco
