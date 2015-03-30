#include "tools-common/compat-nlp-de/stts.h"

#include <sstream>

#include "taco/base/exception.h"

namespace taco {
namespace tool {
namespace de {
namespace stts {

const Tag TagSet::s_adja(0);
const Tag TagSet::s_adjd(1);
const Tag TagSet::s_adv(2);
const Tag TagSet::s_appo(3);
const Tag TagSet::s_appr(4);
const Tag TagSet::s_apprart(5);
const Tag TagSet::s_apzr(6);
const Tag TagSet::s_art(7);
const Tag TagSet::s_card(8);
const Tag TagSet::s_fm(9);
const Tag TagSet::s_itj(10);
const Tag TagSet::s_kokom(11);
const Tag TagSet::s_kon(12);
const Tag TagSet::s_koui(13);
const Tag TagSet::s_kous(14);
const Tag TagSet::s_ne(15);
const Tag TagSet::s_nn(16);
const Tag TagSet::s_ord(17);
const Tag TagSet::s_pav(18);
const Tag TagSet::s_pdat(19);
const Tag TagSet::s_pds(20);
const Tag TagSet::s_piat(21);
const Tag TagSet::s_pidat(22);
const Tag TagSet::s_pis(23);
const Tag TagSet::s_pper(24);
const Tag TagSet::s_pposat(25);
const Tag TagSet::s_pposs(26);
const Tag TagSet::s_prelat(27);
const Tag TagSet::s_prels(28);
const Tag TagSet::s_prf(29);
const Tag TagSet::s_ptka(30);
const Tag TagSet::s_ptkant(31);
const Tag TagSet::s_ptkneg(32);
const Tag TagSet::s_ptkvz(33);
const Tag TagSet::s_ptkzu(34);
const Tag TagSet::s_pwat(35);
const Tag TagSet::s_pwav(36);
const Tag TagSet::s_pws(37);
const Tag TagSet::s_sgml(38);
const Tag TagSet::s_spell(39);
const Tag TagSet::s_trunc(40);
const Tag TagSet::s_vafin(41);
const Tag TagSet::s_vaimp(42);
const Tag TagSet::s_vainf(43);
const Tag TagSet::s_vapp(44);
const Tag TagSet::s_vmfin(45);
const Tag TagSet::s_vminf(46);
const Tag TagSet::s_vmpp(47);
const Tag TagSet::s_vvfin(48);
const Tag TagSet::s_vvimp(49);
const Tag TagSet::s_vvinf(50);
const Tag TagSet::s_vvizu(51);
const Tag TagSet::s_vvpp(52);
const Tag TagSet::s_xy(53);
const Tag TagSet::s_punc_comma(54);
const Tag TagSet::s_punc_final(55);
const Tag TagSet::s_punc_other(56);
const Tag TagSet::s_ns_nne(57);
const Tag TagSet::s_ns_trunc_cap(58);
const Tag TagSet::s_ns_trunc_lc(59);
const Tag TagSet::s_ns_segment(60);
const Tag TagSet::s_ns_junc(61);

TagSet TagSet::s_instance;

bool Tag::IsFiniteVerb() const {
  return (value_ == TagSet::s_vafin.value_ ||
          value_ == TagSet::s_vmfin.value_ ||
          value_ == TagSet::s_vvfin.value_);
}

TagSet::TagSet() {
  // Create bidirectional mapping between tags and strings.
  standard_map_.insert(BiMap::value_type(s_adja, "ADJA"));
  standard_map_.insert(BiMap::value_type(s_adjd, "ADJD"));
  standard_map_.insert(BiMap::value_type(s_adv, "ADV"));
  standard_map_.insert(BiMap::value_type(s_appo, "APPO"));
  standard_map_.insert(BiMap::value_type(s_appr, "APPR"));
  standard_map_.insert(BiMap::value_type(s_apprart, "APPRART"));
  standard_map_.insert(BiMap::value_type(s_apzr, "APZR"));
  standard_map_.insert(BiMap::value_type(s_art, "ART"));
  standard_map_.insert(BiMap::value_type(s_card, "CARD"));
  standard_map_.insert(BiMap::value_type(s_fm, "FM"));
  standard_map_.insert(BiMap::value_type(s_itj, "ITJ"));
  standard_map_.insert(BiMap::value_type(s_kokom, "KOKOM"));
  standard_map_.insert(BiMap::value_type(s_kon, "KON"));
  standard_map_.insert(BiMap::value_type(s_koui, "KOUI"));
  standard_map_.insert(BiMap::value_type(s_kous, "KOUS"));
  standard_map_.insert(BiMap::value_type(s_ne, "NE"));
  standard_map_.insert(BiMap::value_type(s_nn, "NN"));
  standard_map_.insert(BiMap::value_type(s_ord, "ORD"));
  standard_map_.insert(BiMap::value_type(s_pav, "PAV"));
  standard_map_.insert(BiMap::value_type(s_pdat, "PDAT"));
  standard_map_.insert(BiMap::value_type(s_pds, "PDS"));
  standard_map_.insert(BiMap::value_type(s_piat, "PIAT"));
  standard_map_.insert(BiMap::value_type(s_pidat, "PIDAT"));
  standard_map_.insert(BiMap::value_type(s_pis, "PIS"));
  standard_map_.insert(BiMap::value_type(s_pper, "PPER"));
  standard_map_.insert(BiMap::value_type(s_pposat, "PPOSAT"));
  standard_map_.insert(BiMap::value_type(s_pposs, "PPOSS"));
  standard_map_.insert(BiMap::value_type(s_prelat, "PRELAT"));
  standard_map_.insert(BiMap::value_type(s_prels, "PRELS"));
  standard_map_.insert(BiMap::value_type(s_prf, "PRF"));
  standard_map_.insert(BiMap::value_type(s_ptka, "PTKA"));
  standard_map_.insert(BiMap::value_type(s_ptkant, "PTKANT"));
  standard_map_.insert(BiMap::value_type(s_ptkneg, "PTKNEG"));
  standard_map_.insert(BiMap::value_type(s_ptkvz, "PTKVZ"));
  standard_map_.insert(BiMap::value_type(s_ptkzu, "PTKZU"));
  standard_map_.insert(BiMap::value_type(s_pwat, "PWAT"));
  standard_map_.insert(BiMap::value_type(s_pwav, "PWAV"));
  standard_map_.insert(BiMap::value_type(s_pws, "PWS"));
  standard_map_.insert(BiMap::value_type(s_sgml, "SGML"));
  standard_map_.insert(BiMap::value_type(s_spell, "SPELL"));
  standard_map_.insert(BiMap::value_type(s_trunc, "TRUNC"));
  standard_map_.insert(BiMap::value_type(s_vafin, "VAFIN"));
  standard_map_.insert(BiMap::value_type(s_vaimp, "VAIMP"));
  standard_map_.insert(BiMap::value_type(s_vainf, "VAINF"));
  standard_map_.insert(BiMap::value_type(s_vapp, "VAPP"));
  standard_map_.insert(BiMap::value_type(s_vmfin, "VMFIN"));
  standard_map_.insert(BiMap::value_type(s_vminf, "VMINF"));
  standard_map_.insert(BiMap::value_type(s_vmpp, "VMPP"));
  standard_map_.insert(BiMap::value_type(s_vvfin, "VVFIN"));
  standard_map_.insert(BiMap::value_type(s_vvimp, "VVIMP"));
  standard_map_.insert(BiMap::value_type(s_vvinf, "VVINF"));
  standard_map_.insert(BiMap::value_type(s_vvizu, "VVIZU"));
  standard_map_.insert(BiMap::value_type(s_vvpp, "VVPP"));
  standard_map_.insert(BiMap::value_type(s_xy, "XY"));
  standard_map_.insert(BiMap::value_type(s_punc_comma, "$,"));
  standard_map_.insert(BiMap::value_type(s_punc_final, "$."));
  standard_map_.insert(BiMap::value_type(s_punc_other, "$("));
  standard_map_.insert(BiMap::value_type(s_ns_nne, "NNE"));
  standard_map_.insert(BiMap::value_type(s_ns_trunc_cap, "TRUNCCap"));
  standard_map_.insert(BiMap::value_type(s_ns_trunc_lc, "TRUNCLC"));
  standard_map_.insert(BiMap::value_type(s_ns_segment, "SEGMENT"));
  standard_map_.insert(BiMap::value_type(s_ns_junc, "JUNC"));

  // Synonyms
  synonym_map_.insert(Map::value_type("PROAV", s_pav));
  synonym_map_.insert(Map::value_type("PROP", s_pav));
  synonym_map_.insert(Map::value_type("PUNC,", s_punc_comma));
  synonym_map_.insert(Map::value_type("PUNC.", s_punc_final));
  synonym_map_.insert(Map::value_type("PUNCPar", s_punc_other));
}

const std::string &TagSet::TagToString(const Tag &tag) const {
  BiMap::left_const_iterator p = standard_map_.left.find(tag);
  return p->second;
}

Tag TagSet::StringToTag(const std::string &s) const {
  // Try the standard map.
  BiMap::right_const_iterator p = standard_map_.right.find(s);
  if (p != standard_map_.right.end()) {
    return p->second;
  }
  // Try the synonym map.
  Map::const_iterator q = synonym_map_.find(s);
  if (q != synonym_map_.end()) {
    return q->second;
  }
  // Not found...
  std::ostringstream msg;
  msg << "invalid tag: '" << s <<  "'";
  throw Exception(msg.str());
}

const std::string &TagSet::Normalize(const std::string &s) const {
  return TagToString(StringToTag(s));
}

}  // namespace stts
}  // namespace de
}  // namespace tool
}  // namespace taco
