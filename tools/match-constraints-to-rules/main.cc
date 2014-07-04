#include "match_constraints_to_rules.h"

#include "taco/base/exception.h"

#include <sstream>

int main(int argc, char *argv[]) {
  try {
    taco::tool::MatchConstraintsToRules tool;
    return tool.Main(argc, argv);
  } catch (const taco::Exception &e) {
    std::cerr << "unhandled exception: " << e.msg() << std::endl;
    std::exit(1);
  }
}
