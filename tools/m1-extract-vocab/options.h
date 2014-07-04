#ifndef TACO_TOOLS_M1_EXTRACT_VOCAB_OPTIONS_H_
#define TACO_TOOLS_M1_EXTRACT_VOCAB_OPTIONS_H_

namespace taco {
namespace tool {
namespace m1 {

struct Options {
 public:
  Options() {}

  // Positional options.
  std::string input_file;

  // Other options.
  std::string output_file;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
