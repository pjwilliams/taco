#ifndef TACO_TOOLS_M1_EXTRACT_CONSTRAINTS_WRITER_H_
#define TACO_TOOLS_M1_EXTRACT_CONSTRAINTS_WRITER_H_

#include "tree_fragment.h"

#include <string>
#include <vector>

namespace taco {
class ConstraintSetSet;
namespace tool {
class ConstraintExtractWriter;
namespace m1 {

struct TreeFragment;

class Writer {
 public:
  Writer(ConstraintExtractWriter &extract_writer)
      : extract_writer_(extract_writer) {}

  void Write(const TreeFragment &, const CSVec &);

 private:
  void AddBrackets(const std::string &, std::string &) const;

  ConstraintExtractWriter &extract_writer_;
  std::string lhs_;
  std::vector<std::string> rhs_;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
