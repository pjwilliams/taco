#ifndef TACO_TOOLS_COMMON_CLI_TOOL_H_
#define TACO_TOOLS_COMMON_CLI_TOOL_H_

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <boost/program_options/cmdline.hpp>

namespace taco {
namespace tool {

class Tool {
 public:
  virtual ~Tool() {}

  const std::string &name() const { return name_; }

  virtual int Main(int argc, char *argv[]) = 0;

 protected:
  Tool(const std::string &name) : name_(name) {}

  // Returns the boost::program_options style that should be used by all tools.
  static int CommonOptionStyle() {
    namespace cls = boost::program_options::command_line_style;
    return cls::default_style & (~cls::allow_guessing);
  }

  void Warn(const std::string &msg) const {
    std::cerr << name_ << ": warning: " << msg << std::endl;
  }

  void Error(const std::string &msg) const {
    std::cerr << name_ << ": error: " << msg << std::endl;
    std::exit(1);
  }

  // Initialises the tool's main input stream and returns a reference that is
  // valid for the remainder of the tool's lifetime.  If filename is empty or
  // "-" then input is standard input; otherwise it is the named file.  Calls
  // Error() if the file cannot be opened for reading.
  std::istream &OpenInputOrDie(const std::string &filename);

  // Initialises the tool's main output stream and returns a reference that is
  // valid for the remainder of the tool's lifetime.  If filename is empty or
  // "-" then output is standard output; otherwise it is the named file.  Calls
  // Error() if the file cannot be opened for writing.
  std::ostream &OpenOutputOrDie(const std::string &filename);

  // Opens the named input file using the supplied ifstream.  Calls Error() if
  // the file cannot be opened for reading.
  void OpenNamedInputOrDie(const std::string &, std::ifstream &);

  // Opens the named output file using the supplied ofstream.  Calls Error() if
  // the file cannot be opened for writing.
  void OpenNamedOutputOrDie(const std::string &, std::ofstream &);

 private:
  std::string name_;
  std::istream *input_ptr_;
  std::ifstream input_file_stream_;
  std::ostream *output_ptr_;
  std::ofstream output_file_stream_;
};

}  // namespace tool
}  // namespace taco

#endif
