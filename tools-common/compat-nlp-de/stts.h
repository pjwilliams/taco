#ifndef TACO_TOOLS_COMMON_COMPAT_NLP_DE_STTS_H_
#define TACO_TOOLS_COMMON_COMPAT_NLP_DE_STTS_H_

#include <string>

#include <boost/bimap.hpp>
#include <boost/unordered_map.hpp>

#include "tools-common/syntax-tree/string_tree.h"

namespace taco {
namespace tool {
namespace de {
namespace stts {

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

// Set of all part-of-speech tags used in the STTS tagset.  Some non-standard
// tags from variant tagsets (such as BitPar's TRUNCCap) are also included.
// Non-standard tags are distinguished by the prefix "ns" (like s_ns_trunc_cap).
struct TagSet {
 public:
  // Standard STTS tags.
  static const Tag s_adja;
  static const Tag s_adjd;
  static const Tag s_adv;
  static const Tag s_appo;
  static const Tag s_appr;
  static const Tag s_apprart;
  static const Tag s_apzr;
  static const Tag s_art;
  static const Tag s_card;
  static const Tag s_fm;
  static const Tag s_itj;
  static const Tag s_kokom;
  static const Tag s_kon;
  static const Tag s_koui;
  static const Tag s_kous;
  static const Tag s_ne;
  static const Tag s_nn;
  static const Tag s_ord;
  static const Tag s_pav;
  static const Tag s_pdat;
  static const Tag s_pds;
  static const Tag s_piat;
  static const Tag s_pidat;
  static const Tag s_pis;
  static const Tag s_pper;
  static const Tag s_pposat;
  static const Tag s_pposs;
  static const Tag s_prelat;
  static const Tag s_prels;
  static const Tag s_prf;
  static const Tag s_ptka;
  static const Tag s_ptkant;
  static const Tag s_ptkneg;
  static const Tag s_ptkvz;
  static const Tag s_ptkzu;
  static const Tag s_pwat;
  static const Tag s_pwav;
  static const Tag s_pws;
  static const Tag s_sgml;
  static const Tag s_spell;
  static const Tag s_trunc;
  static const Tag s_vafin;
  static const Tag s_vaimp;
  static const Tag s_vainf;
  static const Tag s_vapp;
  static const Tag s_vmfin;
  static const Tag s_vminf;
  static const Tag s_vmpp;
  static const Tag s_vvfin;
  static const Tag s_vvimp;
  static const Tag s_vvinf;
  static const Tag s_vvizu;
  static const Tag s_vvpp;
  static const Tag s_xy;
  static const Tag s_punc_comma;  // $,
  static const Tag s_punc_final;  // $.
  static const Tag s_punc_other;  // $(
  // BitPar-specific tags: these appear in BitPar output, but not, as far
  // as I can tell, in the NEGRA/Tiger literature.
  static const Tag s_ns_nne;
  static const Tag s_ns_trunc_cap;
  static const Tag s_ns_trunc_lc;
  // Special tags used for compound splitting
  static const Tag s_ns_segment;
  static const Tag s_ns_junc;

  // Return the sole instance of this class.
  static const TagSet &Instance() { return s_instance; }

  // Returns a string representation of a Tag object.
  const std::string &TagToString(const Tag &) const;

  // Returns a Tag object given a string representation.  Usually the string
  // must be the standard upper-case STTS name (like "PTKZU") but some
  // synonyms are recognised:
  //
  //   "PROAV"      for   "PAV"     (Tiger)
  //   "PROP"       for   "PAV"
  //   "PUNC,"      for   "$,"      (Moses)
  //   "PUNC."      for   "$."      (Moses)
  //   "PUNCPar"    for   "$("      (Moses)
  //
  Tag StringToTag(const std::string &) const;

  // Return the standard form of the tag string (e.g. "PROAV" becomes "PAV")
  const std::string &Normalize(const std::string &) const;

 private:
  typedef boost::bimap<Tag, std::string> BiMap;
  typedef boost::unordered_map<std::string, Tag> Map;

  // Only a single static variable should be created.
  TagSet();

  static TagSet s_instance;
  BiMap standard_map_;
  Map synonym_map_;
};

}  // namespace stts
}  // namespace de
}  // namespace tool
}  // namespace taco

#endif
