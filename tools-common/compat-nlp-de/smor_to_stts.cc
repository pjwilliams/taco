#include "tools-common/compat-nlp-de/smor_to_stts.h"

#include <cassert>

namespace taco {
namespace tool {
namespace de {

void SmorToSTTS::DerivePosTags(const smor::Analysis &analysis,
                               std::set<stts::Tag> &tags) const {
  using namespace smor;

  tags.clear();

  const std::string &word_class = analysis.word_class.text;
  const Inflection &infl = analysis.inflection;

  if (word_class == "ADJ" || word_class == "ORD") {
    for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
      if (p->text == "Adv" || p->text == "Pred") {
        tags.insert(stts::TagSet::s_adjd);
        return;
      }
    }
    tags.insert(stts::TagSet::s_adja);
    return;
  } else if (word_class == "ART") {
    tags.insert(stts::TagSet::s_art);
    return;
  } else if (word_class == "DEM") {
    bool have_attr_tag = false;
    bool have_pro_tag = false;
    bool have_subst_tag = false;
    for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
      if (p->IsAttr()) {
        have_attr_tag = true;
      } else if (p->IsPro()) {
        have_pro_tag = true;
      } else if (p->IsSubst()) {
        have_subst_tag = true;
      }
    }
    assert(have_attr_tag || have_pro_tag || have_subst_tag);
    if (have_attr_tag || have_pro_tag) {
      tags.insert(stts::TagSet::s_pdat);
    }
    if (have_subst_tag || have_pro_tag) {
      tags.insert(stts::TagSet::s_pds);
    }
    return;
  } else if (word_class == "INDEF") {
    bool have_od = false;
    bool have_md = false;
    for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
      if (p->IsSubst()) {
        tags.insert(stts::TagSet::s_pis);
        return;
      } else if (p->text == "oD" || p->text == "St") {
        have_od = true;
      } else if (p->text == "mD" || p->text == "Wk") {
        have_md = true;
      }
    }
    if ((have_od && !have_md) || (!have_od && have_md)) {
      // FIXME Option to pick Tiger or STTS style?
      // Unlike STTS, the Tiger tagset doesn't make a distinction between
      // forms with and without determiners.
      tags.insert(stts::TagSet::s_piat);
      return;
    }
    // TODO warn
    return;
  } else if (word_class == "POSS") {
    tags.insert(stts::TagSet::s_pposat);
    return;
  } else if (word_class == "PPRO") {
    bool have_pers_tag = false;
    bool have_prfl_tag = false;
    bool have_refl_tag = false;
    bool have_rez_tag = false;
    for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
      if (p->IsPers()) {
        have_pers_tag = true;
      } else if (p->IsPrfl()) {
        have_prfl_tag = true;
      } else if (p->IsRefl()) {
        have_refl_tag = true;
      } else if (p->IsRez()) {
        have_rez_tag = true;
      }
    }
    if (have_pers_tag) {
      assert(!have_prfl_tag);
      assert(!have_refl_tag);
      assert(!have_rez_tag);
      tags.insert(stts::TagSet::s_pper);
      return;
    }
    if (have_prfl_tag) {
      assert(!have_pers_tag);
      assert(!have_refl_tag);
      assert(!have_rez_tag);
      tags.insert(stts::TagSet::s_prf);
      return;
    }
    return;
  } else if (word_class == "PREP") {
    tags.insert(stts::TagSet::s_appr);
    // APPO?
    return;
  } else if (word_class == "PREP/ART" || word_class == "PREPART") {
    tags.insert(stts::TagSet::s_apprart);
    return;
  } else if (word_class == "NN") {
    tags.insert(stts::TagSet::s_nn);
    return;
  } else if (word_class == "SEGMENT") {
    tags.insert(stts::TagSet::s_ns_segment);
    return;
  } else if (word_class == "V") {
    bool have_zu_tag = false;
    bool have_imp_tag = false;
    bool have_inf_tag = false;
    bool have_ppast_tag = false;
    for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
      if (p->text == "Imp") {
        have_imp_tag = true;
      } else if (p->text == "Inf") {
        have_inf_tag = true;
      } else if (p->text == "PPast") {
        have_ppast_tag = true;
      } else if (p->text == "zu") {
        have_zu_tag = true;
      }
    }
    if (have_imp_tag) {
      assert(!have_inf_tag);
      assert(!have_ppast_tag);
      assert(!have_zu_tag);
      tags.insert(stts::TagSet::s_vaimp);
      tags.insert(stts::TagSet::s_vvimp);
      return;
    }
    if (have_inf_tag) {
      assert(!have_ppast_tag);
      if (have_zu_tag) {
        tags.insert(stts::TagSet::s_vvizu);
        return;
      }
      tags.insert(stts::TagSet::s_vvinf);
      tags.insert(stts::TagSet::s_vainf);
      tags.insert(stts::TagSet::s_vminf);
      return;
    }
    if (have_ppast_tag) {
      tags.insert(stts::TagSet::s_vvpp);
      tags.insert(stts::TagSet::s_vapp);
      tags.insert(stts::TagSet::s_vmpp);
      return;
    }
    tags.insert(stts::TagSet::s_vvfin);
    tags.insert(stts::TagSet::s_vafin);
    tags.insert(stts::TagSet::s_vmfin);
    return;
  } else if (word_class == "WPRO") {
    for (Inflection::const_iterator p(infl.begin()); p != infl.end(); ++p) {
      if (p->IsSubst()) {
        tags.insert(stts::TagSet::s_pws);
        return;
      }
    }
    tags.insert(stts::TagSet::s_pwat);
    return;
  } else {
    return;
  }
}

}  // namespace de
}  // namespace tool
}  // namespace taco
