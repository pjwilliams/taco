#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "tools-common/compat-nlp-de/bitpar.h"

BOOST_AUTO_TEST_CASE(test_bitpar_label_parser) {
  taco::tool::de::BitParLabelParser parser;
  taco::tool::de::BitParLabel label;

  // Cat
  {
    std::string s = "PUNC,";
    parser.Parse(s, label);
    BOOST_CHECK(label.cat == "PUNC,");
    BOOST_CHECK(label.func.empty());
    BOOST_CHECK(label.morph.empty());
    BOOST_CHECK(label.slash.empty());
  }

  // Cat + slash
  {
    std::string s = "PUNC./imp";
    parser.Parse(s, label);
    BOOST_CHECK(label.cat == "PUNC.");
    BOOST_CHECK(label.func.empty());
    BOOST_CHECK(label.morph.empty());
    BOOST_CHECK(label.slash.size() == 1);
    BOOST_CHECK(label.slash[0] == "imp");
  }

  // Cat + func
  {
    std::string s = "ADV-MO";
    parser.Parse(s, label);
    BOOST_CHECK(label.cat == "ADV");
    BOOST_CHECK(label.func == "MO");
    BOOST_CHECK(label.morph.empty());
    BOOST_CHECK(label.slash.empty());
  }

  // Cat + func + slash
  {
    std::string s = "AP-CJ/pred";
    parser.Parse(s, label);
    BOOST_CHECK(label.cat == "AP");
    BOOST_CHECK(label.func == "CJ");
    BOOST_CHECK(label.morph.empty());
    BOOST_CHECK(label.slash.size() == 1);
    BOOST_CHECK(label.slash[0] == "pred");
  }

  // Cat + func + slash + slash
  {
    std::string s = "S-TOP/nohead/nosubj";
    parser.Parse(s, label);
    BOOST_CHECK(label.cat == "S");
    BOOST_CHECK(label.func == "TOP");
    BOOST_CHECK(label.morph.empty());
    BOOST_CHECK(label.slash.size() == 2);
    BOOST_CHECK(label.slash[0] == "nohead");
    BOOST_CHECK(label.slash[1] == "nosubj");
  }

  // Cat + func + morph
  {
    std::string s = "ART-NK-Nom.Sg.Fem";
    parser.Parse(s, label);
    BOOST_CHECK(label.cat == "ART");
    BOOST_CHECK(label.func == "NK");
    BOOST_CHECK(label.morph.size() == 3);
    BOOST_CHECK(label.morph[0] == "Nom");
    BOOST_CHECK(label.morph[1] == "Sg");
    BOOST_CHECK(label.morph[2] == "Fem");
    BOOST_CHECK(label.slash.empty());
  }

  // Cat + func + mysterious number + morph
  {
    std::string s = "PN-DA-2-Dat.Sg.Masc";
    parser.Parse(s, label);
    BOOST_CHECK(label.cat == "PN");
    BOOST_CHECK(label.func == "DA");
    BOOST_CHECK(label.morph.size() == 3);
    BOOST_CHECK(label.morph[0] == "Dat");
    BOOST_CHECK(label.morph[1] == "Sg");
    BOOST_CHECK(label.morph[2] == "Masc");
    BOOST_CHECK(label.slash.empty());
  }

  // Cat + func + morph + slash
  {
    std::string s = "AP-HD-Dat.Pl.Fem/nom";
    parser.Parse(s, label);
    BOOST_CHECK(label.cat == "AP");
    BOOST_CHECK(label.func == "HD");
    BOOST_CHECK(label.morph.size() == 3);
    BOOST_CHECK(label.morph[0] == "Dat");
    BOOST_CHECK(label.morph[1] == "Pl");
    BOOST_CHECK(label.morph[2] == "Fem");
    BOOST_CHECK(label.slash.size() == 1);
    BOOST_CHECK(label.slash[0] == "nom");
  }

  // TODO Add checks for empty and malformed labels
}
