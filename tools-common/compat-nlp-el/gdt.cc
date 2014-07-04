#include "tools-common/compat-nlp-el/gdt.h"

#include <sstream>

#include "taco/base/exception.h"

namespace taco {
namespace tool {
namespace el {
namespace gdt {

const Tag TagSet::s_ad(0);
const Tag TagSet::s_aj(1);
const Tag TagSet::s_as_pp_pa(2);
const Tag TagSet::s_as_pp_sp(3);
const Tag TagSet::s_at_df(4);
const Tag TagSet::s_at_id(5);
const Tag TagSet::s_comp(6);
const Tag TagSet::s_cj_co(7);
const Tag TagSet::s_cj_sb(8);
const Tag TagSet::s_date(9);
const Tag TagSet::s_dig(10);
const Tag TagSet::s_enum(11);
const Tag TagSet::s_init(12);
const Tag TagSet::s_lsplit(13);
const Tag TagSet::s_punct(14);
const Tag TagSet::s_nm_cd(15);
const Tag TagSet::s_nm_ct(16);
const Tag TagSet::s_nm_ml(17);
const Tag TagSet::s_nm_od(18);
const Tag TagSet::s_no_cm(19);
const Tag TagSet::s_no_pr(20);
const Tag TagSet::s_pn_dm(21);
const Tag TagSet::s_pn_id(22);
const Tag TagSet::s_pn_ir(23);
const Tag TagSet::s_pn_pe(24);
const Tag TagSet::s_pn_po(25);
const Tag TagSet::s_pn_re(26);
const Tag TagSet::s_pn_ri(27);
const Tag TagSet::s_pt_fu(28);
const Tag TagSet::s_pt_ng(29);
const Tag TagSet::s_pt_ot(30);
const Tag TagSet::s_pt_sj(31);
const Tag TagSet::s_rg_ab_xx(32);
const Tag TagSet::s_rg_an_xx(33);
const Tag TagSet::s_rg_fw_or(34);
const Tag TagSet::s_rg_fw_tr(35);
const Tag TagSet::s_vb_is(36);
const Tag TagSet::s_vb_mn(37);

TagSet TagSet::s_instance;

TagSet::TagSet() {
  // Create bidirectional mapping between tags and strings.
  standard_map_.insert(BiMap::value_type(s_ad, "Ad"));
  standard_map_.insert(BiMap::value_type(s_aj, "Aj"));
  standard_map_.insert(BiMap::value_type(s_as_pp_pa, "AsPpPa"));
  standard_map_.insert(BiMap::value_type(s_as_pp_sp, "AsPpSp"));
  standard_map_.insert(BiMap::value_type(s_at_df, "AtDf"));
  standard_map_.insert(BiMap::value_type(s_at_id, "AtId"));
  standard_map_.insert(BiMap::value_type(s_comp, "COMP"));
  standard_map_.insert(BiMap::value_type(s_cj_co, "CjCo"));
  standard_map_.insert(BiMap::value_type(s_cj_sb, "CjSp"));
  standard_map_.insert(BiMap::value_type(s_date, "DATE"));
  standard_map_.insert(BiMap::value_type(s_dig, "DIG"));
  standard_map_.insert(BiMap::value_type(s_enum, "ENUM"));
  standard_map_.insert(BiMap::value_type(s_init, "INIT"));
  standard_map_.insert(BiMap::value_type(s_lsplit, "LSPLIT"));
  standard_map_.insert(BiMap::value_type(s_punct, "PUNCT"));
  standard_map_.insert(BiMap::value_type(s_nm_cd, "NmCd"));
  standard_map_.insert(BiMap::value_type(s_nm_ct, "NmCt"));
  standard_map_.insert(BiMap::value_type(s_nm_ml, "NmMl"));
  standard_map_.insert(BiMap::value_type(s_nm_od, "NmOd"));
  standard_map_.insert(BiMap::value_type(s_no_cm, "NoCm"));
  standard_map_.insert(BiMap::value_type(s_no_pr, "NoPr"));
  standard_map_.insert(BiMap::value_type(s_pn_dm, "PnDm"));
  standard_map_.insert(BiMap::value_type(s_pn_id, "PnId"));
  standard_map_.insert(BiMap::value_type(s_pn_ir, "PnIr"));
  standard_map_.insert(BiMap::value_type(s_pn_pe, "PnPe"));
  standard_map_.insert(BiMap::value_type(s_pn_po, "PnPo"));
  standard_map_.insert(BiMap::value_type(s_pn_re, "PnRe"));
  standard_map_.insert(BiMap::value_type(s_pn_ri, "PnRi"));
  standard_map_.insert(BiMap::value_type(s_pt_fu, "PtFu"));
  standard_map_.insert(BiMap::value_type(s_pt_ng, "PtNg"));
  standard_map_.insert(BiMap::value_type(s_pt_ot, "PtOt"));
  standard_map_.insert(BiMap::value_type(s_pt_sj, "PtSj"));
  standard_map_.insert(BiMap::value_type(s_rg_ab_xx, "RgAbXx"));
  standard_map_.insert(BiMap::value_type(s_rg_an_xx, "RgAnXx"));
  standard_map_.insert(BiMap::value_type(s_rg_fw_or, "RgFwOr"));
  standard_map_.insert(BiMap::value_type(s_rg_fw_tr, "RgFwTr"));
  standard_map_.insert(BiMap::value_type(s_vb_is, "VbIs"));
  standard_map_.insert(BiMap::value_type(s_vb_mn, "VbMn"));
}

const std::string &TagSet::TagToString(const Tag &tag) const {
  BiMap::left_const_iterator p = standard_map_.left.find(tag);
  return p->second;
}

Tag TagSet::StringToTag(const std::string &s) const {
  BiMap::right_const_iterator p = standard_map_.right.find(s);
  if (p != standard_map_.right.end()) {
    return p->second;
  }
  // Not found...
  std::ostringstream msg;
  msg << "invalid tag: '" << s <<  "'";
  throw Exception(msg.str());
}

}  // namespace gdt
}  // namespace el
}  // namespace tool
}  // namespace taco
