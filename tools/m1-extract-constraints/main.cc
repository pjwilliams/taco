#include "m1_extract_constraints.h"

#include "taco/base/exception.h"

#include <sstream>

int main(int argc, char *argv[]) {
  try {
    taco::tool::m1::ExtractConstraints tool;
    return tool.Main(argc, argv);
  } catch (const taco::Exception &e) {
    std::cerr << "unhandled exception: " << e.msg() << std::endl;
    std::exit(1);
  }
}
