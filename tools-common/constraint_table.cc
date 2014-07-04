#include "tools-common/constraint_table.h"

#include "tools-common/text-formats/constraint_map_parser.h"

#include "taco/base/string_piece.h"
#include "taco/base/string_util.h"
#include "taco/text-formats/constraint_set_parser.h"
#include "taco/text-formats/constraint_table_parser.h"

#include <istream>
#include <string>
#include <vector>

namespace taco {
namespace tool {

const ConstraintSetSet *ConstraintTable::Lookup(const Key &key) const {
  const_iterator p = table_.find(key);
  return (p == table_.end()) ? 0 : &(*p->second);
}

void ConstraintTable::Insert(const Key &key,
                             boost::shared_ptr<ConstraintSetSet> css) {
  table_[key] = css;
}

void BasicConstraintTableLoader::Load(std::istream &map_stream,
                                      std::istream &table_stream,
                                      ConstraintTable &table) const {
  ConstraintSetParser cs_parser(feature_set_, value_set_);

  std::map<unsigned int, boost::shared_ptr<ConstraintSet> > cs_map;

  ConstraintTableParser end;
  for (ConstraintTableParser parser(table_stream); parser != end; ++parser) {
    const ConstraintTableParser::Entry &entry = *parser;
    unsigned int id = std::atoi(entry.id.as_string().c_str());
    cs_map[id] = cs_parser.Parse(entry.constraint_set);
  }

  typedef DereferencingOrderer<boost::shared_ptr<ConstraintSetSet>,
                               ConstraintSetSetOrderer> Orderer;
  typedef std::set<boost::shared_ptr<ConstraintSetSet>, Orderer> CSSSet;

  std::vector<StringPiece> tokens;
  CSSSet css_set;
  ConstraintTable::Key key;

  ConstraintMapParser end2;
  for (ConstraintMapParser parser(map_stream); parser != end2; ++parser) {
    const ConstraintMapParser::Entry &entry = *parser;

    key.clear();
    tokens.clear();
    Tokenize(tokens, entry.key, "-");
    for (std::vector<StringPiece>::const_iterator p = tokens.begin();
         p != tokens.end(); ++p) {
      key.push_back(std::atoi((*p).as_string().c_str()));
    }

    boost::shared_ptr<ConstraintSetSet> css(new ConstraintSetSet());
    for (std::vector<StringPiece>::const_iterator p = entry.ids.begin();
         p != entry.ids.end(); ++p) {
      int id = std::atoi((*p).as_string().c_str());
      // TODO Check ID is known.
      css->insert(cs_map[id]);
    }

    std::pair<CSSSet::iterator, bool> result = css_set.insert(css);
    table.Insert(key, *result.first);
  }
}

}  // namespace tool
}  // namespace taco
