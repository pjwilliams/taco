#ifndef TACO_TOOLS_M1_EXTRACT_LEXICON_INTERPRETER_H_
#define TACO_TOOLS_M1_EXTRACT_LEXICON_INTERPRETER_H_

#include "options.h"
#include "pos_table.h"

#include "tools-common/compat-nlp-de/smor.h"
#include "tools-common/compat-nlp-de/stts.h"

#include "taco/base/vocabulary.h"
#include "taco/feature_structure.h"
#include "taco/lexicon.h"

#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace taco {
namespace tool {
namespace m1 {

class Interpreter {
 public:
  Interpreter(const Options &, Vocabulary &, Vocabulary &, Vocabulary &,
              const PosTable &, const BasicLexiconWriter &, std::ostream &);

  // FIXME This should be a callback set by the client
  void Warn(const std::string &) const;

  void operator()(const de::smor::WordAnalysesPair &);

 private:
  typedef std::set<FeatureStructureSpec> FSSpecSet;

  class WarningCallback {
   public:
    WarningCallback(const Interpreter &interpreter, const std::string &prefix)
        : interpreter_(interpreter)
        , prefix_(prefix) {}

    void operator()(const std::string &msg) const {
      interpreter_.Warn(prefix_+msg);
    }

   private:
    const Interpreter &interpreter_;
    std::string prefix_;
  };

  void InterpretAdjInflection(const de::smor::Inflection &,
                              const WarningCallback &,
                              FSSpecSet &);

  void InterpretArtInflection(const de::smor::Inflection &,
                              const WarningCallback &,
                              FSSpecSet &);

  void InterpretDemInflection(const de::smor::Inflection &,
                              const WarningCallback &,
                              FSSpecSet &);

  void InterpretIndefInflection(const de::smor::Inflection &,
                                const WarningCallback &,
                                FSSpecSet &);

  void InterpretNnInflection(const de::smor::Inflection &,
                             const WarningCallback &,
                             FSSpecSet &);

  void InterpretOrdInflection(const de::smor::Inflection &,
                              const WarningCallback &,
                              FSSpecSet &);

  void InterpretPossInflection(const de::smor::Inflection &,
                               const WarningCallback &,
                               FSSpecSet &);

  void InterpretPproInflection(const de::smor::Inflection &,
                               const WarningCallback &,
                               FSSpecSet &);

  void InterpretPrepInflection(const de::smor::Inflection &,
                               const WarningCallback &,
                               FSSpecSet &);

  void InterpretPrepArtInflection(const de::smor::Inflection &,
                                  const WarningCallback &,
                                  FSSpecSet &);

  void InterpretVInflection(const de::smor::Inflection &,
                            const WarningCallback &,
                            FSSpecSet &);

  void InterpretWproInflection(const de::smor::Inflection &,
                               const WarningCallback &,
                               FSSpecSet &);

  FeatureStructureSpec CreateInflSpec(AtomicValue, AtomicValue, AtomicValue,
                                      AtomicValue, AtomicValue);

  static void FiddleArtStem(de::smor::Stem &);

  bool MatchCase(const de::smor::Feature &, AtomicValue &);
  bool MatchDecl(const de::smor::Feature &, bool &, bool &, bool &);
  bool MatchDefiniteness(const de::smor::Feature &, bool &, bool &);
  bool MatchGender(const de::smor::Feature &, AtomicValue &);
  bool MatchImperative(const de::smor::Feature &);
  bool MatchInfinite(const de::smor::Feature &);
  bool MatchMood(const de::smor::Feature &, AtomicValue &);
  bool MatchNumber(const de::smor::Feature &, AtomicValue &);
  bool MatchPastParticiple(const de::smor::Feature &);
  bool MatchPerson(const de::smor::Feature &, AtomicValue &);
  bool MatchPresentParticiple(const de::smor::Feature &);
  bool MatchTense(const de::smor::Feature &, AtomicValue &);

  const Options &options_;
  Vocabulary &vocabulary_;
  Vocabulary &feature_set_;
  Vocabulary &value_set_;
  const PosTable &pos_table_;
  const BasicLexiconWriter &lexicon_writer_;
  std::ostream &output_;
  std::set<de::stts::Tag> relation_pos_tags_;
};

}  // namespace m1
}  // namespace tool
}  // namespace taco

#endif
