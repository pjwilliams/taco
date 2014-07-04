#include "prune_redundant_constraints.h"

int main(int argc, char *argv[]) {
  taco::tool::PruneRedundantConstraints tool;
  return tool.Main(argc, argv);
}
