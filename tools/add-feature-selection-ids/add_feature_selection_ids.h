#ifndef TACO_TOOLS_ADD_FEATURE_SELECTION_IDS_ADD_FEATURE_SELECTION_IDS_H_
#define TACO_TOOLS_ADD_FEATURE_SELECTION_IDS_ADD_FEATURE_SELECTION_IDS_H_

#include "tools-common/cli/tool.h"

#include "taco/base/string_piece.h"
#include "taco/base/vocabulary.h"

#include <string>
#include <utility>
#include <vector>

namespace taco {
namespace tool {

struct Options;

class AddFeatureSelectionIds : public Tool {
 public:
  AddFeatureSelectionIds() : Tool("add-feature-selection-ids") {}
  virtual int Main(int, char *[]);
 private:
  void ProcessOptions(int, char *[], Options &) const;
  void ConstructFeatureMapKey(const StringPiece &,
                              std::pair<std::string, std::string> &) const;
};

}  // namespace tool
}  // namespace taco

#endif
