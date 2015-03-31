#include "interpreter.h"

#include "tools-common/compat-nlp-de/smor_to_stts.h"
#include "tools-common/compat-nlp-de/stts.h"

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace taco {
namespace tool {
namespace m1 {

Interpreter::Interpreter(const Options &options,
                         Vocabulary &vocabulary,
                         Vocabulary &feature_set,
                         Vocabulary &value_set,
                         const PosTable &pos_table,
                         const BasicLexiconWriter &lexicon_writer,
                         std::ostream &output)
    : options_(options)
    , vocabulary_(vocabulary)
    , feature_set_(feature_set)
    , value_set_(value_set)
    , pos_table_(pos_table)
    , lexicon_writer_(lexicon_writer)
    , output_(output) {
  // FIXME Remove relation_pos_tags_ and produce an entry for any POS?
  relation_pos_tags_.insert(de::stts::TagSet::s_adja);
  relation_pos_tags_.insert(de::stts::TagSet::s_appr);
  relation_pos_tags_.insert(de::stts::TagSet::s_apprart);
  relation_pos_tags_.insert(de::stts::TagSet::s_art);
  relation_pos_tags_.insert(de::stts::TagSet::s_pdat);
  relation_pos_tags_.insert(de::stts::TagSet::s_pds);
  relation_pos_tags_.insert(de::stts::TagSet::s_piat);
  relation_pos_tags_.insert(de::stts::TagSet::s_pidat);
  relation_pos_tags_.insert(de::stts::TagSet::s_pper);
  relation_pos_tags_.insert(de::stts::TagSet::s_pposat);
  relation_pos_tags_.insert(de::stts::TagSet::s_pwat);
  relation_pos_tags_.insert(de::stts::TagSet::s_nn);
  relation_pos_tags_.insert(de::stts::TagSet::s_ns_segment);
  relation_pos_tags_.insert(de::stts::TagSet::s_vafin);
  relation_pos_tags_.insert(de::stts::TagSet::s_vmfin);
  relation_pos_tags_.insert(de::stts::TagSet::s_vvfin);
  relation_pos_tags_.insert(de::stts::TagSet::s_vvinf);
  relation_pos_tags_.insert(de::stts::TagSet::s_vvizu);
  relation_pos_tags_.insert(de::stts::TagSet::s_vvpp);
}

void Interpreter::Warn(const std::string &msg) const {
  std::cerr << "warning: " << msg << std::endl;
}

void Interpreter::operator()(const de::smor::WordAnalysesPair &pair) {
  const std::string &word = pair.first;
  const de::smor::Analyses &analyses = pair.second;

  // Get the set of POS tags observed for this word in the corpus.
  size_t word_id = vocabulary_.Insert(word);
  PosTable::const_iterator p = pos_table_.find(word_id);
  if (p == pos_table_.end()) {
    std::ostringstream msg;
    msg << "analysed word not found in vocab file: `" << word << "'";
    Warn(msg.str());
    return;
  }
  const std::set<de::stts::Tag> &corpus_pos_tags = p->second;

  FSSpecSet specs;
  std::set<de::stts::Tag> analysis_pos_tags;

  const de::stts::TagSet &tag_set = de::stts::TagSet::Instance();
  de::SmorToSTTS smor_to_stts;

  size_t analysis_num = 0;
  for (de::smor::Analyses::const_iterator p(analyses.begin());
       p != analyses.end(); ++p) {
    const de::smor::Analysis &analysis = *p;
    ++analysis_num;

    // Derive the set of Tiger POS tags that correspond to this analysis.
    std::set<de::stts::Tag> pos_tags;
    smor_to_stts.DerivePosTags(analysis, pos_tags);
    if (pos_tags.empty()) {
      // TODO Is this normal?  Warn?
      continue;
    }
    bool match = false;
    for (std::set<de::stts::Tag>::const_iterator q = pos_tags.begin();
         q != pos_tags.end(); ++q) {
      const de::stts::Tag &pos_tag = *q;
      analysis_pos_tags.insert(pos_tag);
      if ((corpus_pos_tags.find(pos_tag) != corpus_pos_tags.end()) &&
           relation_pos_tags_.find(pos_tag) != relation_pos_tags_.end()) {
        match = true;
      }
    }
    if (!match) {
      // If this analysis doesn't correspond to any POS tag observed in the
      // corpus or if the tag will never occur in a relation then there's
      // no point creating a lexicon entry.
      continue;
    }

    FSSpecSet infl_specs;

    std::ostringstream msg;
    msg << "analysis " << analysis_num << " of `" << word << "': ";
    WarningCallback callback(*this, msg.str());

    try {
      if (analysis.word_class.text == "ADJ") {
        InterpretAdjInflection(analysis.inflection, callback, infl_specs);
      } else if (analysis.word_class.text == "ART") {
        InterpretArtInflection(analysis.inflection, callback, infl_specs);
      } else if (analysis.word_class.text == "DEM") {
        InterpretDemInflection(analysis.inflection, callback, infl_specs);
      } else if (analysis.word_class.text == "INDEF") {
        InterpretIndefInflection(analysis.inflection, callback, infl_specs);
      } else if (analysis.word_class.text == "NN" ||
                 analysis.word_class.text == "SEGMENT") {
        InterpretNnInflection(analysis.inflection, callback, infl_specs);
      } else if (analysis.word_class.text == "ORD") {
        InterpretOrdInflection(analysis.inflection, callback, infl_specs);
      } else if (analysis.word_class.text == "POSS") {
        InterpretPossInflection(analysis.inflection, callback, infl_specs);
      } else if (analysis.word_class.text == "PPRO") {
        InterpretPproInflection(analysis.inflection, callback, infl_specs);
      } else if (analysis.word_class.text == "PREP") {
        InterpretPrepInflection(analysis.inflection, callback, infl_specs);
      } else if (analysis.word_class.text == "PREP/ART" ||
                 analysis.word_class.text == "PREPART") {
        InterpretPrepArtInflection(analysis.inflection, callback, infl_specs);
      } else if (analysis.word_class.text == "V") {
        InterpretVInflection(analysis.inflection, callback, infl_specs);
      } else if (analysis.word_class.text == "WPRO") {
        InterpretWproInflection(analysis.inflection, callback, infl_specs);
      } else {
        assert(false);
      }
    } catch (const Exception &e) {
      std::ostringstream msg;
      msg << "failed to interpret analysis " << analysis_num
          << " of `" << word << "': " << e.msg();
      Warn(msg.str());
      continue;
    }

    for (FSSpecSet::const_iterator q(infl_specs.begin());
         q != infl_specs.end(); ++q) {
      const FeatureStructureSpec &infl_spec = *q;
      for (std::set<de::stts::Tag>::const_iterator r(pos_tags.begin());
           r != pos_tags.end(); ++r) {
        // TODO direct map from Tag to AtomicValue
        const std::string &tag_string = tag_set.TagToString(*r);
        AtomicValue pos_value = value_set_.Insert(tag_string);
        FeatureStructureSpec spec;
        FeaturePath path;
        path.push_back(feature_set_.Insert("INFL"));
        spec.Nest(path, infl_spec);
        path[0] = feature_set_.Insert("CAT");
        spec.content_pairs.insert(std::make_pair(path, pos_value));
        specs.insert(spec);
      }
    }
  }

  // Create a feature structure for each POS tag that appears in the corpus but
  // not in the analyses.  This also catches words that SMOR failed to analyse.
  // The resulting feature structure will be empty except for the POS value.
  for (std::set<de::stts::Tag>::const_iterator q(relation_pos_tags_.begin());
       q != relation_pos_tags_.end(); ++q) {

    if ((corpus_pos_tags.find(*q) != corpus_pos_tags.end()) &&
        analysis_pos_tags.find(*q) == analysis_pos_tags.end()) {
      // TODO direct map from Tag to AtomicValue
      const std::string &tag_string = tag_set.TagToString(*q);
      AtomicValue pos_value = value_set_.Insert(tag_string);
      FeatureStructureSpec spec;
      FeaturePath path;
      path.push_back(feature_set_.Insert("CAT"));
      spec.content_pairs.insert(std::make_pair(path, pos_value));
      specs.insert(spec);
    }
  }

  for (FSSpecSet::const_iterator p(specs.begin()); p != specs.end(); ++p) {
    boost::shared_ptr<FeatureStructure> fs(new FeatureStructure(*p));
    lexicon_writer_.WriteLine(word, *fs, output_);
  }
}

FeatureStructureSpec Interpreter::CreateInflSpec(
    AtomicValue case_val,
    AtomicValue decl_val,
    AtomicValue gender_val,
    AtomicValue number_val,
    AtomicValue person_val=kNullAtom) {
  FeatureStructureSpec spec;
  FeaturePath path;

  if (case_val != kNullAtom) {
    path.resize(1);
    path[0] = feature_set_.Insert("CASE");
    spec.content_pairs.insert(std::make_pair(path, case_val));
  }

  if (decl_val != kNullAtom && !options_.disable_decl_feature) {
    path.resize(1);
    path[0] = feature_set_.Insert("DECL");
    spec.content_pairs.insert(std::make_pair(path, decl_val));
  }

  if (gender_val != kNullAtom && number_val != value_set_.Insert("pl")) {
    path.resize(2);
    path[0] = feature_set_.Insert("AGR");
    path[1] = feature_set_.Insert("GENDER");
    spec.content_pairs.insert(std::make_pair(path, gender_val));
  }

  if (number_val != kNullAtom) {
    path.resize(2);
    path[0] = feature_set_.Insert("AGR");
    path[1] = feature_set_.Insert("NUMBER");
    spec.content_pairs.insert(std::make_pair(path, number_val));
  }

  if (person_val != kNullAtom) {
    path.resize(2);
    path[0] = feature_set_.Insert("AGR");
    path[1] = feature_set_.Insert("PERSON");
    spec.content_pairs.insert(std::make_pair(path, person_val));
  }

  if (spec.content_pairs.empty()) {
    spec.content_pairs.insert(std::make_pair(FeaturePath(), kNullAtom));
  }

  return spec;
}

void Interpreter::InterpretOrdInflection(const de::smor::Inflection &infl,
                                         const WarningCallback &callback,
                                         FSSpecSet &specs) {
  using de::smor::Inflection;

  bool weak = false;
  bool mixed = false;
  bool strong = false;
  AtomicValue case_val = kNullAtom;
  AtomicValue gender_val = kNullAtom;
  AtomicValue number_val = kNullAtom;

  for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
    if (p->text == "Pred") {
      specs.insert(CreateInflSpec(kNullAtom, kNullAtom, kNullAtom, kNullAtom));
      return;
    }
    MatchCase(*p, case_val)
      || MatchGender(*p, gender_val)
      || MatchNumber(*p, number_val)
      || MatchDecl(*p, weak, mixed, strong);
  }

  if (case_val == kNullAtom) {
    callback("missing case value");
  }
  if (gender_val == kNullAtom && number_val != value_set_.Insert("pl")) {
    callback("missing gender value");
  }
  if (number_val == kNullAtom) {
    callback("missing number value");
  }

  if (!strong && !mixed && !weak) {
    specs.insert(CreateInflSpec(case_val, kNullAtom, gender_val, number_val));
  } else {
    if (strong) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("strong"),
                                  gender_val, number_val));
    }
    if (mixed) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("mixed"),
                                  gender_val, number_val));
    }
    if (weak) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("weak"),
                                  gender_val, number_val));
    }
    // Newer versions of SMOR don't use a mixed feature.
    if (strong && !mixed && number_val == value_set_.Insert("sg") &&
        (case_val == value_set_.Insert("nom") ||
         case_val == value_set_.Insert("acc"))) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("mixed"),
                                  gender_val, number_val));
    }
    if (weak && !mixed &&
        (number_val == value_set_.Insert("pl") ||
         case_val == value_set_.Insert("dat") ||
         case_val == value_set_.Insert("gen"))) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("mixed"),
                                  gender_val, number_val));
    }
  }
}

void Interpreter::InterpretAdjInflection(const de::smor::Inflection &infl,
                                         const WarningCallback &callback,
                                         FSSpecSet &specs) {
  using de::smor::Inflection;

  bool weak = false;
  bool mixed = false;
  bool strong = false;
  AtomicValue case_val = kNullAtom;
  AtomicValue gender_val = kNullAtom;
  AtomicValue number_val = kNullAtom;

  for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
    if (p->text == "Invar" || p->text == "Pred" || p->text == "Adv") {
      specs.insert(CreateInflSpec(kNullAtom, kNullAtom, kNullAtom, kNullAtom));
      return;
    }
    if (p->text == "Pos" || p->text == "Comp" || p->text == "Sup") {
      continue;
    }
    MatchCase(*p, case_val)
      || MatchGender(*p, gender_val)
      || MatchNumber(*p, number_val)
      || MatchDecl(*p, weak, mixed, strong);
  }

  if (case_val == kNullAtom) {
    callback("missing case value");
  }
  if (gender_val == kNullAtom && number_val != value_set_.Insert("pl")) {
    callback("missing gender value");
  }
  if (number_val == kNullAtom) {
    callback("missing number value");
  }

  if (!strong && !mixed && !weak) {
    specs.insert(CreateInflSpec(case_val, kNullAtom, gender_val, number_val));
  } else {
    if (strong) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("strong"),
                                  gender_val, number_val));
    }
    if (mixed) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("mixed"),
                                  gender_val, number_val));
    }
    if (weak) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("weak"),
                                  gender_val, number_val));
    }
    // Newer versions of SMOR don't use a mixed feature.
    if (strong && !mixed && number_val == value_set_.Insert("sg") &&
        (case_val == value_set_.Insert("nom") ||
         case_val == value_set_.Insert("acc"))) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("mixed"),
                                  gender_val, number_val));
    }
    if (weak && !mixed &&
        (number_val == value_set_.Insert("pl") ||
         case_val == value_set_.Insert("dat") ||
         case_val == value_set_.Insert("gen"))) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("mixed"),
                                  gender_val, number_val));
    }
  }
}

void Interpreter::InterpretArtInflection(const de::smor::Inflection &infl,
                                         const WarningCallback &callback,
                                         FSSpecSet &specs) {
  using de::smor::Inflection;

  bool definite = false;
  bool indefinite = false;
  AtomicValue case_val = kNullAtom;
  AtomicValue gender_val = kNullAtom;
  AtomicValue number_val = kNullAtom;

  for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
    MatchCase(*p, case_val)
      || MatchGender(*p, gender_val)
      || MatchNumber(*p, number_val)
      || MatchDefiniteness(*p, definite, indefinite);
  }

  if (case_val == kNullAtom) {
    callback("missing case value");
  }
  if (gender_val == kNullAtom && number_val != value_set_.Insert("pl")) {
    callback("missing gender value");
  }
  if (number_val == kNullAtom) {
    callback("missing number value");
  }
  if (!definite && !indefinite) {
    callback("missing definiteness value");
  }

  if (indefinite) {
    specs.insert(CreateInflSpec(case_val, value_set_.Insert("mixed"),
                                gender_val, number_val));
  } else if (definite) {
    specs.insert(CreateInflSpec(case_val, value_set_.Insert("weak"),
                                gender_val, number_val));
  } else {
    specs.insert(CreateInflSpec(case_val, kNullAtom, gender_val, number_val));
  }
}

void Interpreter::InterpretNnInflection(const de::smor::Inflection &infl,
                                        const WarningCallback &callback,
                                        FSSpecSet &specs) {
  using de::smor::Inflection;

  bool weak = false;
  bool mixed = false;
  bool strong = false;
  AtomicValue case_val = kNullAtom;
  AtomicValue gender_val = kNullAtom;
  AtomicValue number_val = kNullAtom;
  AtomicValue person_val = value_set_.Insert("3");  // Always third person.

  for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
    MatchCase(*p, case_val)
      || MatchGender(*p, gender_val)
      || MatchNumber(*p, number_val)
      || MatchDecl(*p, weak, mixed, strong);
  }

  if (case_val == kNullAtom) {
    callback("missing case value");
  }
  if (gender_val == kNullAtom && number_val != value_set_.Insert("pl")) {
    callback("missing gender value");
  }
  if (number_val == kNullAtom) {
    callback("missing number value");
  }

  if (!strong && !mixed && !weak) {
    specs.insert(CreateInflSpec(case_val, kNullAtom, gender_val, number_val,
                                person_val));
  } else {
    if (strong) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("strong"),
                                  gender_val, number_val, person_val));
    }
    if (mixed) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("mixed"),
                                  gender_val, number_val, person_val));
    }
    if (weak) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("weak"),
                                  gender_val, number_val, person_val));
    }
    // Newer versions of SMOR don't use a mixed feature.
    if (strong && !mixed && number_val == value_set_.Insert("sg") &&
        (case_val == value_set_.Insert("nom") ||
         case_val == value_set_.Insert("acc"))) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("mixed"),
                                  gender_val, number_val));
    }
    if (weak && !mixed &&
        (number_val == value_set_.Insert("pl") ||
         case_val == value_set_.Insert("dat") ||
         case_val == value_set_.Insert("gen"))) {
      specs.insert(CreateInflSpec(case_val, value_set_.Insert("mixed"),
                                  gender_val, number_val));
    }
  }
}

void Interpreter::InterpretIndefInflection(const de::smor::Inflection &infl,
                                           const WarningCallback &callback,
                                           FSSpecSet &specs) {
  using de::smor::Inflection;

  AtomicValue case_val = kNullAtom;
  AtomicValue gender_val = kNullAtom;
  AtomicValue number_val = kNullAtom;

  for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
    MatchCase(*p, case_val)
      || MatchGender(*p, gender_val)
      || MatchNumber(*p, number_val);
  }

  if (case_val == kNullAtom) {
    callback("missing case value");
  }
  if (gender_val == kNullAtom && number_val != value_set_.Insert("pl")) {
    callback("missing gender value");
  }
  if (number_val == kNullAtom) {
    callback("missing number value");
  }

  specs.insert(CreateInflSpec(case_val, kNullAtom, gender_val, number_val));
}

void Interpreter::InterpretDemInflection(const de::smor::Inflection &infl,
                                         const WarningCallback &callback,
                                         FSSpecSet &specs) {
  using de::smor::Inflection;

  AtomicValue case_val = kNullAtom;
  AtomicValue gender_val = kNullAtom;
  AtomicValue number_val = kNullAtom;
  AtomicValue person_val = value_set_.Insert("3");  // Always third person.

  for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
    MatchCase(*p, case_val)
      || MatchGender(*p, gender_val)
      || MatchNumber(*p, number_val);
  }

  if (case_val == kNullAtom) {
    callback("missing case value");
  }
  if (gender_val == kNullAtom && number_val != value_set_.Insert("pl")) {
    callback("missing gender value");
  }
  if (number_val == kNullAtom) {
    callback("missing number value");
  }

  // TODO Declension?  A DEM can be a PDAT (mixed?) or a PDS (strong).  The
  // TODO POS tag depends on whether the DEM has a attr, pro, or subst tag
  // TODO (see SmorToSTTS::DeriveTags()).  If a DEM has a pro tag then
  // TODO it can produce a PDAT and a PDS, so perhaps the declension should
  // TODO be set elsewhere, based on the actual POS tag...
  specs.insert(CreateInflSpec(case_val, kNullAtom, gender_val, number_val,
                              person_val));
}

void Interpreter::InterpretPossInflection(const de::smor::Inflection &infl,
                                          const WarningCallback &callback,
                                          FSSpecSet &specs) {
  using de::smor::Inflection;

  AtomicValue case_val = kNullAtom;
  AtomicValue gender_val = kNullAtom;
  AtomicValue number_val = kNullAtom;

  for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
    MatchCase(*p, case_val)
      || MatchGender(*p, gender_val)
      || MatchNumber(*p, number_val);
  }

  if (case_val == kNullAtom) {
    callback("missing case value");
  }
  if (gender_val == kNullAtom && number_val != value_set_.Insert("pl")) {
    callback("missing gender value");
  }
  if (number_val == kNullAtom) {
    callback("missing number value");
  }

  specs.insert(CreateInflSpec(case_val, value_set_.Insert("mixed"), gender_val,
                              number_val));
}

void Interpreter::InterpretPproInflection(const de::smor::Inflection &infl,
                                          const WarningCallback &callback,
                                          FSSpecSet &specs) {
  using de::smor::Inflection;

  AtomicValue case_val = kNullAtom;
  AtomicValue gender_val = kNullAtom;
  AtomicValue number_val = kNullAtom;
  AtomicValue person_val = kNullAtom;

  for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
    MatchCase(*p, case_val)
      || MatchGender(*p, gender_val)
      || MatchNumber(*p, number_val)
      || MatchPerson(*p, person_val);
  }

  if (case_val == kNullAtom) {
    callback("missing case value");
  }
  if (gender_val == kNullAtom && number_val != value_set_.Insert("pl")) {
    callback("missing gender value");
  }
  if (number_val == kNullAtom) {
    callback("missing number value");
  }
  if (person_val == kNullAtom) {
    callback("missing person value");
  }

  specs.insert(CreateInflSpec(case_val, kNullAtom, gender_val, number_val,
                              person_val));
}

void Interpreter::InterpretPrepInflection(const de::smor::Inflection &infl,
                                          const WarningCallback &callback,
                                          FSSpecSet &specs) {
  using de::smor::Inflection;

  AtomicValue case_val = kNullAtom;

  // TODO Check for unexpected values

  for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
    if (MatchCase(*p, case_val)) {
      continue;
    }
  }

  if (case_val == kNullAtom) {
    callback("missing case value");
  }

  // Suppress case value, if requested.
  if (options_.disable_prep_case) {
    case_val = kNullAtom;
  }

  specs.insert(CreateInflSpec(case_val, kNullAtom, kNullAtom, kNullAtom));
}

void Interpreter::InterpretPrepArtInflection(
    const de::smor::Inflection &infl,
    const WarningCallback &callback,
    FSSpecSet &specs) {
  using de::smor::Inflection;

  AtomicValue case_val = kNullAtom;
  AtomicValue gender_val = kNullAtom;
  AtomicValue number_val = kNullAtom;

  for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
    MatchCase(*p, case_val)
      || MatchGender(*p, gender_val)
      || MatchNumber(*p, number_val);
  }

  if (case_val == kNullAtom) {
    callback("missing case value");
  }
  if (gender_val == kNullAtom && number_val != value_set_.Insert("pl")) {
    callback("missing gender value");
  }
  if (number_val == kNullAtom) {
    callback("missing number value");
  }

  specs.insert(CreateInflSpec(case_val, value_set_.Insert("weak"), gender_val,
                              number_val));
}


void Interpreter::InterpretVInflection(const de::smor::Inflection &infl,
                                       const WarningCallback &callback,
                                       FSSpecSet &a_specs) {
  using de::smor::Inflection;

  bool imperative = false;
  bool infinite = false;
  bool past_participle = false;
  bool present_participle = false;

  AtomicValue mood_val = kNullAtom;
  AtomicValue number_val = kNullAtom;
  AtomicValue tense_val = kNullAtom;
  AtomicValue person_val = kNullAtom;

  for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
    if (MatchMood(*p, mood_val)) {
      continue;
    }
    if (MatchNumber(*p, number_val)) {
      continue;
    }
    if (MatchTense(*p, tense_val)) {
      continue;
    }
    if (MatchPerson(*p, person_val)) {
      continue;
    }
    if (MatchImperative(*p)) {
      imperative = true;
      continue;
    }
    if (MatchInfinite(*p)) {
      infinite = true;
      continue;
    }
    if (MatchPastParticiple(*p)) {
      past_participle = true;
      continue;
    }
    if (MatchPresentParticiple(*p)) {
      present_participle = true;
      continue;
    }
  }

  // Check some assumptions about what features can occur in a single analysis.
  if (infinite || past_participle || present_participle) {
    if (number_val != kNullAtom) {
      callback("unexpected number value");
    }
  } else if (number_val == kNullAtom) {
    callback("missing number value");
  }
  if (infinite && past_participle) {
    callback("verb has both infinite and part-participle features");
  }
  if (infinite && present_participle) {
    callback("verb has both infinite and present-participle features");
  }
  if (past_participle && present_participle) {
    callback("verb has both past-present_participle and present-participle features");
  }
  if (past_participle && present_participle) {
    callback("verb has both past-participle and present-participle features");
  }

  // Create agreement specs.
  if (!infinite && !past_participle && !present_participle) {
    a_specs.insert(CreateInflSpec(kNullAtom, kNullAtom, kNullAtom, number_val,
                                  person_val));
  } else {
    a_specs.insert(CreateInflSpec(kNullAtom, kNullAtom, kNullAtom, kNullAtom));
  }
}

void Interpreter::InterpretWproInflection(const de::smor::Inflection &infl,
                                          const WarningCallback &callback,
                                          FSSpecSet &specs) {
  using de::smor::Inflection;

  AtomicValue case_val = kNullAtom;
  AtomicValue gender_val = kNullAtom;
  AtomicValue number_val = kNullAtom;

  for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
    MatchCase(*p, case_val)
      || MatchGender(*p, gender_val)
      || MatchNumber(*p, number_val);
  }

  if (case_val == kNullAtom) {
    callback("missing case value");
  }
  if (gender_val == kNullAtom && number_val != value_set_.Insert("pl")) {
    callback("missing gender value");
  }
  if (number_val == kNullAtom) {
    callback("missing number value");
  }

  specs.insert(CreateInflSpec(case_val, kNullAtom, gender_val, number_val));
}

void Interpreter::FiddleArtStem(de::smor::Stem &stem) {
  using de::smor::Stem;
  for (Stem::iterator p(stem.begin()); p != stem.end(); ++p) {
    if (p->is_feature) {
      continue;
    }
    if (p->text == "ein") {
      continue;
    } else if (p->text == "der" || p->text == "die" || p->text == "das") {
      p->text = "der";
    } else {
      std::ostringstream msg;
      msg << "Unexpected morpheme in ART stem: " << p->text;
      throw Exception(msg.str());
    }
  }
}

bool Interpreter::MatchCase(const de::smor::Feature &feature,
                            AtomicValue &case_val) {
  if (feature.IsNom()) {
    if (case_val != kNullAtom) {
      throw Exception("multiple case values");
    }
    case_val = value_set_.Insert("nom");
    return true;
  }
  if (feature.IsAcc()) {
    if (case_val != kNullAtom) {
      throw Exception("multiple case values");
    }
    case_val = value_set_.Insert("acc");
    return true;
  }
  if (feature.IsGen()) {
    if (case_val != kNullAtom) {
      throw Exception("multiple case values");
    }
    case_val = value_set_.Insert("gen");
    return true;
  }
  if (feature.IsDat()) {
    if (case_val != kNullAtom) {
      throw Exception("multiple case values");
    }
    case_val = value_set_.Insert("dat");
    return true;
  }
  return false;
}

bool Interpreter::MatchDecl(const de::smor::Feature &feature, bool &weak,
                            bool &mixed, bool &strong) {
  if (feature.IsSt()) {
    if (weak || mixed || strong) {
      throw Exception("multiple declension features");
    }
    strong = true;
    return true;
  }
  if (feature.IsStMix()) {
    if (weak || mixed || strong) {
      throw Exception("multiple declension features");
    }
    strong = mixed = true;
    return true;
  }
  if (feature.IsWk()) {
    if (weak || mixed || strong) {
      throw Exception("multiple declension features");
    }
    weak = true;
    return true;
  }
  if (feature.IsWkMix()) {
    if (weak || mixed || strong) {
      throw Exception("multiple declension features");
    }
    weak = mixed = true;
    return true;
  }
  return false;
}

bool Interpreter::MatchDefiniteness(const de::smor::Feature &feature,
                                    bool &definite, bool &indefinite) {
  if (feature.text == "Def") {
    if (definite || indefinite) {
      throw Exception("multiple definiteness features");
    }
    definite = true;
    return true;
  }
  if (feature.text == "Indef") {
    if (definite || indefinite) {
      throw Exception("multiple definiteness features");
    }
    indefinite = true;
    return true;
  }
  return false;
}

bool Interpreter::MatchGender(const de::smor::Feature &feature,
                              AtomicValue &gender_val) {
  if (feature.text == "Masc") {
    if (gender_val != kNullAtom) {
      throw Exception("multiple gender values");
    }
    gender_val = value_set_.Insert("m");
    return true;
  }
  if (feature.text == "Fem") {
    if (gender_val != kNullAtom) {
      throw Exception("multiple gender values");
    }
    gender_val = value_set_.Insert("f");
    return true;
  }
  if (feature.text == "Neut") {
    if (gender_val != kNullAtom) {
      throw Exception("multiple gender values");
    }
    gender_val = value_set_.Insert("n");
    return true;
  }
  return false;
}

bool Interpreter::MatchImperative(const de::smor::Feature &feature) {
  return feature.text == "Imp";
}

bool Interpreter::MatchInfinite(const de::smor::Feature &feature) {
  return feature.text == "Inf";
}

bool Interpreter::MatchMood(const de::smor::Feature &feature,
                            AtomicValue &mood_val) {
  if (feature.text == "Ind") {
    if (mood_val != kNullAtom) {
      throw Exception("multiple mood values");
    }
    mood_val = value_set_.Insert("indicative");
    return true;
  }
  if (feature.text == "Konj" || feature.text == "Subj") {
    if (mood_val != kNullAtom) {
      throw Exception("multiple mood values");
    }
    mood_val = value_set_.Insert("subjunctive");
    return true;
  }
  return false;
}

bool Interpreter::MatchNumber(const de::smor::Feature &feature,
                              AtomicValue &number_val) {
  if (feature.text == "Sg") {
    if (number_val != kNullAtom) {
      throw Exception("multiple number values");
    }
    number_val = value_set_.Insert("sg");
    return true;
  }
  if (feature.text == "Pl") {
    if (number_val != kNullAtom) {
      throw Exception("multiple number values");
    }
    number_val = value_set_.Insert("pl");
    return true;
  }
  return false;
}

bool Interpreter::MatchPerson(const de::smor::Feature &feature,
                              AtomicValue &person_val) {
  if (feature.text == "1") {
    if (person_val != kNullAtom) {
      throw Exception("multiple person values");
    }
    person_val = value_set_.Insert("1");
    return true;
  }
  if (feature.text == "2") {
    if (person_val != kNullAtom) {
      throw Exception("multiple person values");
    }
    person_val = value_set_.Insert("2");
    return true;
  }
  if (feature.text == "3") {
    if (person_val != kNullAtom) {
      throw Exception("multiple person values");
    }
    person_val = value_set_.Insert("3");
    return true;
  }
  return false;
}

bool Interpreter::MatchTense(const de::smor::Feature &feature,
                             AtomicValue &tense_val) {
  if (feature.text == "Past") {
    if (tense_val != kNullAtom) {
      throw Exception("multiple tense values");
    }
    tense_val = value_set_.Insert("past");
    return true;
  }
  if (feature.text == "Pres") {
    if (tense_val != kNullAtom) {
      throw Exception("multiple tense values");
    }
    tense_val = value_set_.Insert("present");
    return true;
  }
  return false;
}

bool Interpreter::MatchPastParticiple(const de::smor::Feature &feature) {
  return feature.text == "PPast";
}

bool Interpreter::MatchPresentParticiple(const de::smor::Feature &feature) {
  return feature.text == "PPres";
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
