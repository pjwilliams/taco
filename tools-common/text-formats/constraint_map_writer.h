#ifndef TACO_TOOLS_COMMON_TEXT_FORMATS_CONSTRAINT_MAP_WRITER_H_
#define TACO_TOOLS_COMMON_TEXT_FORMATS_CONSTRAINT_MAP_WRITER_H_

#include <ostream>
#include <vector>

namespace taco {
namespace tool {

class ConstraintMapWriter {
 public:
  ConstraintMapWriter(std::ostream &output) : output_(output) {}

  // SymbolType must provide operator<<(std::ostream &, SymbolType).
  // Typically it will be std::string or taco::StringPiece.
  // IdType must provide operator<<(std::ostream &, IdType).
  // Typically it will be std::string or an integral type.
  template<typename KeyType, typename IdType>
  void Write(const KeyType &key, const std::vector<IdType> &ids) const;

 private:
  std::ostream &output_;
};

template<typename KeyType, typename IdType>
void ConstraintMapWriter::Write(const KeyType &key,
                                const std::vector<IdType> &ids) const {
  output_ << key << " |||";
  for (typename std::vector<IdType>::const_iterator p = ids.begin();
       p != ids.end(); ++p) {
    output_ << " " << *p;
  }
  output_ << std::endl;
}

}  // namespace tool
}  // namespace taco

#endif
