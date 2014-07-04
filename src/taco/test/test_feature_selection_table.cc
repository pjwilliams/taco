#include <boost/test/unit_test.hpp>

#include "taco/feature_selection_table.h"

#include "taco/base/vocabulary.h"

#include <sstream>

BOOST_AUTO_TEST_CASE(testFeatureSelectionTableParser) {
  using namespace taco;

  std::stringstream s;
  s << "0 ||| assign" << std::endl
    << "1 ||| drop" << std::endl
    << "2 ||| [AGREEMENT:[NUMBER]]" << std::endl;

  FeatureSelectionTable table;

  Vocabulary feature_set;

  FeatureSelectionTableParser end;
  int i = 0;
  for (FeatureSelectionTableParser parser(s, feature_set);
       parser != end; ++parser) {
    const FeatureSelectionTableParser::Entry &entry = *parser;
    BOOST_CHECK(entry.index == i++);
    table.push_back(entry.rule);
  }

  BOOST_CHECK(table.size() == 3);
  BOOST_CHECK(table[0]->type == FeatureSelectionRule::Rule_Assign);
  BOOST_CHECK(table[0]->selection_tree == 0);
  BOOST_CHECK(table[1]->type == FeatureSelectionRule::Rule_Drop);
  BOOST_CHECK(table[1]->selection_tree == 0);
  BOOST_CHECK(table[2]->type == FeatureSelectionRule::Rule_Select);
  BOOST_CHECK(table[2]->selection_tree != 0);

  boost::shared_ptr<FeatureTree> ft = table[2]->selection_tree;
  BOOST_CHECK(ft->children_.size() == 1);
  boost::shared_ptr<FeatureTree> child = ft->children_[feature_set.Lookup("AGREEMENT")];
  BOOST_CHECK(child->children_.size() == 1);
  boost::shared_ptr<FeatureTree> grandchild = child->children_[feature_set.Lookup("NUMBER")];
  BOOST_CHECK(grandchild->children_.empty());
}
