#ifndef TACO_TOOLS_COMMON_COMPAT_NLP_EL_GDT_H_
#define TACO_TOOLS_COMMON_COMPAT_NLP_EL_GDT_H_

#include <string>

#include <boost/bimap.hpp>
#include <boost/unordered_map.hpp>

#include "tools-common/syntax-tree/string_tree.h"

namespace taco {
namespace tool {
namespace el {
namespace gdt {

// Immutable type representing a part-of-speech tag.  A Tag object can only be
// created by calling TagSet::StringToTag(), by copying, or by assignment.
struct Tag {
 public:
  typedef char ValueType;
 public:
  Tag(const Tag &other) : value_(other.value_) {}
  Tag &operator=(const Tag &tag) {
    value_ = tag.value_;
    return *this;
  }
  bool operator<(const Tag &other) const { return value_ < other.value_; }
  bool IsFiniteVerb() const;
 private:
  friend struct TagSet;
  explicit Tag(ValueType v) : value_(v) {}
  ValueType value_;
};

// Set of all part-of-speech tags used in the GDT tagset.
struct TagSet {
 public:
  // Standard GDT tags.
  static const Tag s_ad;
  static const Tag s_aj;
  static const Tag s_as_pp_pa;
  static const Tag s_as_pp_sp;
  static const Tag s_at_df;
  static const Tag s_at_id;
  static const Tag s_comp;
  static const Tag s_cj_co;
  static const Tag s_cj_sb;
  static const Tag s_date;
  static const Tag s_dig;
  static const Tag s_enum;
  static const Tag s_init;
  static const Tag s_lsplit;
  static const Tag s_punct;
  static const Tag s_nm_cd;
  static const Tag s_nm_ct;
  static const Tag s_nm_ml;
  static const Tag s_nm_od;
  static const Tag s_no_cm;
  static const Tag s_no_pr;
  static const Tag s_pn_dm;
  static const Tag s_pn_id;
  static const Tag s_pn_ir;
  static const Tag s_pn_pe;
  static const Tag s_pn_po;
  static const Tag s_pn_re;
  static const Tag s_pn_ri;
  static const Tag s_pt_fu;
  static const Tag s_pt_ng;
  static const Tag s_pt_ot;
  static const Tag s_pt_sj;
  static const Tag s_rg_ab_xx;
  static const Tag s_rg_an_xx;
  static const Tag s_rg_fw_or;
  static const Tag s_rg_fw_tr;
  static const Tag s_vb_is;
  static const Tag s_vb_mn;

  // Return the sole instance of this class.
  static const TagSet &Instance() { return s_instance; }

  // Returns a string representation of a Tag object.
  const std::string &TagToString(const Tag &) const;

  // Returns a Tag object given a string representation.
  Tag StringToTag(const std::string &) const;

 private:
  typedef boost::bimap<Tag, std::string> BiMap;

  // Only a single static variable should be created.
  TagSet();

  static TagSet s_instance;
  BiMap standard_map_;
};

}  // namespace gdt
}  // namespace el
}  // namespace tool
}  // namespace taco

#endif
