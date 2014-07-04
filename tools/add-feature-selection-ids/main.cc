#include "add_feature_selection_ids.h"

int main(int argc, char *argv[]) {
  taco::tool::AddFeatureSelectionIds tool;
  return tool.Main(argc, argv);
}
