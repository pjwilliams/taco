#include "pos_table.h"

#include "tools-common/compat-nlp-de/bitpar.h"
#include "tools-common/compat-nlp-de/stts.h"
#include "tools-common/text-formats/vocab_parser.h"

#include "taco/base/exception.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <istream>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace taco {
namespace tool {
namespace m1 {

void PosTableLoader::Load(std::istream &input, PosTable &pos_table) {
  const de::stts::TagSet &tag_set = de::stts::TagSet::Instance();
  de::BitParLabelParser label_parser;
  std::string line;
  size_t line_num = 0;
  VocabParser end;
  for (VocabParser parser(input); parser != end; ++parser) {
    const VocabParser::Entry &entry = *parser;
    ++line_num;
    const size_t len = entry.symbol.size();
    if (len > 2 && entry.symbol[0] == '[' && entry.symbol[len-1] == ']') {
      continue;
    }
    size_t word_id = vocab_.Insert(entry.symbol.as_string());

    std::set<de::stts::Tag> pos_tags;
    for (std::vector<StringPiece>::const_iterator p(entry.pos_set.begin());
         p != entry.pos_set.end(); ++p) {
      de::BitParLabel parsed_label;
      label_parser.Parse(p->as_string(), parsed_label);
      try {
        de::stts::Tag tag = tag_set.StringToTag(parsed_label.cat);
        pos_tags.insert(tag);
      } catch (const Exception &e) {
        std::ostringstream msg;
        msg << "line " << line_num << ": " << e.msg();
        throw Exception(msg.str());
      }
    }

    pos_table.insert(std::make_pair(word_id, pos_tags));
  }
}

}  // namespace m1
}  // namespace tool
}  // namespace taco
