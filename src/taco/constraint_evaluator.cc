#include "taco/constraint_evaluator.h"

#include <algorithm>

#include "taco/constraint_set.h"
#include "taco/interpretation.h"
#include "taco/option_table.h"

namespace taco {

bool ConstraintEvaluator::Eval(const OptionTable &option_table,
                               const ConstraintSet &constraint_set,
                               std::vector<Interpretation> &results) const {
  results.clear();

  OptionTable::const_iterator p = option_table.begin();

  // TODO What's the right thing to do here?
  if (p == option_table.end()) {
    return true;
  }

  // Create initial Interpretations, one for each feature structure option
  // of the first rule element in option_table.
  {
    size_t index = p->first;
    const OptionColumn &col = p->second;
    results.reserve(col.Size());
    for (OptionColumn::const_iterator q = col.begin(); q != col.end(); ++q) {
      boost::shared_ptr<const FeatureStructure> fs = *q;
      assert(fs);
      assert(fs->IsComplex());
      Interpretation interpretation(index, fs->Clone());
      if (interpretation.Eval(constraint_set)) {
        results.push_back(interpretation);
      }
    }
    if (results.empty()) {
      return false;
    }
  }

  std::vector<Interpretation> new_results;

  // Iteratively extend Interpretations to cover remaining rule elements.
  for (++p; p != option_table.end(); ++p) {
    size_t index = p->first;
    const OptionColumn &col = p->second;
    for (OptionColumn::const_iterator q = col.begin(); q != col.end(); ++q) {
      boost::shared_ptr<const FeatureStructure> fs = *q;
      assert(fs);
      std::vector<Interpretation>::const_iterator r;
      std::vector<Interpretation>::const_iterator end = results.end();
      for (r = results.begin(); r != end; ++r) {
        PotentialInterpretation candidate(*r, index, fs);
        if (!candidate.QuickCheck(constraint_set)) {
          continue;
        }
        Interpretation interpretation(candidate);
        if (interpretation.Eval(constraint_set)) {
          new_results.push_back(interpretation);
        }
      }
    }
    if (new_results.empty()) {
      results.clear();
      return false;
    }
    results.swap(new_results);
    new_results.clear();
  }

  return !results.empty();
}

bool ConstraintEvaluator::Eval(const std::vector<Interpretation> &prev_results,
                               const OptionTable &option_table,
                               const ConstraintSet &constraint_set,
                               std::vector<Interpretation> &results) const {
  // TODO Avoid copying?
  results = prev_results;

  // TODO What's the right thing to do here?
  // Normally the resulting feature structures would be independent of those
  // in prev_results (due to cloning) but in this case they'll be shared
  // pointers to same objects.
  if (option_table.IsEmpty()) {
    assert(false);
    return true;
  }

  std::vector<Interpretation> new_results;

  // Iteratively expand Interpretations to cover new rule elements.
  for (OptionTable::const_iterator p = option_table.begin();
       p != option_table.end(); ++p) {
    size_t index = p->first;
    const OptionColumn &col = p->second;
    for (OptionColumn::const_iterator q = col.begin(); q != col.end(); ++q) {
      boost::shared_ptr<const FeatureStructure> fs = *q;
      assert(fs);
      std::vector<Interpretation>::const_iterator r;
      std::vector<Interpretation>::const_iterator end = results.end();
      for (r = results.begin(); r != end; ++r) {
        PotentialInterpretation candidate(*r, index, fs);
        if (!candidate.QuickCheck(constraint_set)) {
          continue;
        }
        Interpretation interpretation(candidate);
        // FIXME Is constraint evaluation necessary?
        if (!interpretation.IsEmpty() && interpretation.Eval(constraint_set)) {
          new_results.push_back(interpretation);
        }
      }
    }
    if (new_results.empty()) {
      results.clear();
      return false;
    }
    results.swap(new_results);
    new_results.clear();
  }

  return !results.empty();
}

// TODO This should be an optimised version of the other eval() -- only need
// to find first valid interpretation.
bool ConstraintEvaluator::Eval(const OptionTable &option_table,
                               const ConstraintSet &constraint_set) const {
  std::vector<Interpretation> results;
  return Eval(option_table, constraint_set, results);
}

}  // namespace taco
