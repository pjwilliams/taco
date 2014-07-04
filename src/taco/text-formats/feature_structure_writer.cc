#include "taco/text-formats/feature_structure_writer.h"

namespace taco {

void FeatureStructureWriter::Write(const FeatureStructure &fs,
                                   std::ostream &out) const {
  if (fs.IsAtomic()) {
    out << value_set_.Lookup(fs.GetAtomicValue());
    return;
  }

  out << "[";
  bool first = true;
  Feature feature = 0;
  for (Vocabulary::const_iterator p(feature_set_.begin());
       p != feature_set_.end(); ++p, ++feature) {
    const std::string &feature_string = **p;
    boost::shared_ptr<const FeatureStructure> value = fs.Get(feature);
    if (!value) {
      continue;
    }
    if (first) {
      first = false;
    } else {
      out << ";";
    }
    out << feature_string;
    out << ":";
    Write(*value, out);
  }
  out << "]";
}

}  // namespace taco
