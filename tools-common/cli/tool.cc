#include "tool.h"

#include <sstream>

namespace taco {
namespace tool {

std::istream &Tool::OpenInputOrDie(const std::string &filename) {
  // TODO Check that function is only called once?
  if (filename.empty() || filename == "-") {
    input_ptr_ = &(std::cin);
  } else {
    input_file_stream_.open(filename.c_str());
    if (!input_file_stream_) {
      std::ostringstream msg;
      msg << "failed to open input file: " << filename;
      Error(msg.str());
    }
    input_ptr_ = &input_file_stream_;
  }
  return *input_ptr_;
}

std::ostream &Tool::OpenOutputOrDie(const std::string &filename) {
  // TODO Check that function is only called once?
  if (filename.empty() || filename == "-") {
    output_ptr_ = &(std::cout);
  } else {
    output_file_stream_.open(filename.c_str());
    if (!output_file_stream_) {
      std::ostringstream msg;
      msg << "failed to open output file: " << filename;
      Error(msg.str());
    }
    output_ptr_ = &output_file_stream_;
  }
  return *output_ptr_;
}

void Tool::OpenNamedInputOrDie(const std::string &filename,
                               std::ifstream &stream) {
  stream.open(filename.c_str());
  if (!stream) {
    std::ostringstream msg;
    msg << "failed to open input file: " << filename;
    Error(msg.str());
  }
}

void Tool::OpenNamedOutputOrDie(const std::string &filename,
                                std::ofstream &stream) {
  stream.open(filename.c_str());
  if (!stream) {
    std::ostringstream msg;
    msg << "failed to open output file: " << filename;
    Error(msg.str());
  }
}

}  // namespace tool
}  // namespace taco
