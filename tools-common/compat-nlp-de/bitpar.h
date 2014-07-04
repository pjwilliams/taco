#ifndef TACO_TOOLS_COMMON_COMPAT_NLP_DE_BITPAR_H_
#define TACO_TOOLS_COMMON_COMPAT_NLP_DE_BITPAR_H_

#include <string>
#include <vector>

namespace taco {
namespace tool {
namespace de {

struct BitParLabel {
  void Clear();
  std::string cat;                 // Part-of-speech / phrase category
  std::string func;                // Syntactic function
  std::vector<std::string> morph;  // Morphological information
  std::vector<std::string> slash;  // Slash features
  // TODO Flag to distinguish POS labels from phrasal labels?
};

class BitParLabelParser {
 public:
  void Parse(const std::string &, BitParLabel &) const;
 private:
  static bool IsNumeric(const std::string &);
  void RemoveMysteriousNumbers() const;
  // Variables are used as temporaries by parse().  They have been made members
  // to avoid repeated allocation on every call to parse.
// FIXME THREAD SAFETY?  Use boost::thread_specific_ptr to store them?
  mutable std::vector<std::string> parts_;
  mutable std::string pre_slash_;
  mutable std::string slash_string_;
};

}  // namespace de
}  // namespace tool
}  // namespace taco

#endif
