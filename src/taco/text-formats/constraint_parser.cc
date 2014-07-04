#include "taco/text-formats/constraint_parser.h"

#include <cassert>
#include <cctype>
#include <sstream>

#include <boost/lexical_cast.hpp>

#include "taco/base/exception.h"

namespace taco {
namespace internal {

ConstraintParserBase::ConstraintParserBase()
    : feature_set_(0)
    , value_set_(0)
    , tokeniser_()
    , lookahead_(ConstraintToken_EOS, "", -1) {
}

ConstraintParserBase::ConstraintParserBase(Vocabulary &feature_set,
                                           Vocabulary &value_set)
    : feature_set_(&feature_set)
    , value_set_(&value_set)
    , tokeniser_()
    , lookahead_(ConstraintToken_EOS, "", -1) {
}

void ConstraintParserBase::Match(ConstraintTokenType expected_type) {
  if (lookahead_.type != expected_type) {
    std::ostringstream msg;
    msg << "Unexpected token: expected "
        << GetConstraintTokenTypeName(expected_type)
        << ", got "
        << GetConstraintTokenTypeName(lookahead_.type);
    throw Exception(msg.str());
  }
  lookahead_ = *(++tokeniser_);
}

void ConstraintParserBase::Match(ConstraintTokenType expected_type,
                                 std::string &value) {
  if (lookahead_.type != expected_type) {
    std::ostringstream msg;
    msg << "Unexpected token: expected "
        << GetConstraintTokenTypeName(expected_type)
        << ", got "
        << GetConstraintTokenTypeName(lookahead_.type);
    throw Exception(msg.str());
  }
  value = lookahead_.value;
  lookahead_ = *(++tokeniser_);
}

boost::shared_ptr<Constraint> ConstraintParserBase::NTConstraint(
    ConstraintType &constraint_type) {
  if (lookahead_.type == ConstraintToken_LANGLE) {
    // The first term is a PathTerm.  The Constraint could be an
    // AbsConstraint, a RelConstraint, or a VarConstraint.
    PathTerm lhs = NTPathTerm();
    Match(ConstraintToken_EQUALS);
    if (lookahead_.type == ConstraintToken_STRING) {
      ValueTerm rhs = NTValueTerm();
      constraint_type = kAbsConstraint;
      return boost::shared_ptr<Constraint>(new AbsConstraint(lhs, rhs));
    } else if (lookahead_.type == ConstraintToken_LANGLE) {
      PathTerm rhs = NTPathTerm();
      constraint_type = kRelConstraint;
      return boost::shared_ptr<Constraint>(new RelConstraint(lhs, rhs));
    } else if (lookahead_.type == ConstraintToken_LCURLY) {
      VarTerm rhs = NTVarTerm();
      constraint_type = kVarConstraint;
      return boost::shared_ptr<Constraint>(new VarConstraint(lhs, rhs));
    } else {
      // TODO throw exception
      assert(false);
    }
  } else if (lookahead_.type == ConstraintToken_STRING) {
    // Must be an AbsConstraint.
    ValueTerm lhs = NTValueTerm();
    Match(ConstraintToken_EQUALS);
    if (lookahead_.type == ConstraintToken_LANGLE) {
      PathTerm rhs = NTPathTerm();
      constraint_type = kAbsConstraint;
      return boost::shared_ptr<Constraint>(new AbsConstraint(rhs, lhs));
    } else {
      // TODO throw exception
      assert(false);
    }
  } else if (lookahead_.type == ConstraintToken_LCURLY) {
    // Must be a VarConstraint.
    VarTerm lhs = NTVarTerm();
    Match(ConstraintToken_EQUALS);
    if (lookahead_.type == ConstraintToken_LANGLE) {
      PathTerm rhs = NTPathTerm();
      constraint_type = kVarConstraint;
      return boost::shared_ptr<Constraint>(new VarConstraint(rhs, lhs));
    } else {
      // TODO throw exception
      assert(false);
    }
  } else {
    // TODO throw exception
    assert(false);
  }
}

PathTerm ConstraintParserBase::NTPathTerm() {
  Match(ConstraintToken_LANGLE);
  Match(ConstraintToken_DIGITSEQ, tmp_value_);
  int index = std::atoi(tmp_value_.c_str());
  FeaturePath path;
  NTPath(path);
  Match(ConstraintToken_RANGLE);
  return PathTerm(index, path);
}

ValueTerm ConstraintParserBase::NTValueTerm() {
  Match(ConstraintToken_STRING, tmp_value_);
  AtomicValue atom = value_set_->Insert(tmp_value_);
  return ValueTerm(atom);
}

VarTerm ConstraintParserBase::NTVarTerm() {
  Match(ConstraintToken_LCURLY);
  VarTerm::ProbabilityMap prob_map;
  NTValProbList(prob_map);
  VarTerm term(prob_map);
  Match(ConstraintToken_RCURLY);
  return term;
}

void ConstraintParserBase::NTPath(FeaturePath &path) {
  if (lookahead_.type == ConstraintToken_STRING) {
    Match(ConstraintToken_STRING, tmp_value_);
    path.push_back(feature_set_->Insert(tmp_value_));
    NTPath(path);
  }
}

void ConstraintParserBase::NTValProbList(VarTerm::ProbabilityMap &prob_map) {
  if (lookahead_.type == ConstraintToken_STRING) {
    Match(ConstraintToken_STRING, tmp_value_);
    AtomicValue val = value_set_->Insert(tmp_value_);
    Match(ConstraintToken_COLON);
    Match(ConstraintToken_PROBABILITY, tmp_value_);
    try {
      float prob = boost::lexical_cast<float>(tmp_value_);
      prob_map[val] = prob;
    } catch (const boost::bad_lexical_cast &e) {
      std::ostringstream msg;
      msg << "invalid constraint probability: " << tmp_value_;
      throw Exception(msg.str());
    }
    NTValProbListTail(prob_map);
  }
}

void ConstraintParserBase::NTValProbListTail(
    VarTerm::ProbabilityMap &prob_map) {
  if (lookahead_.type == ConstraintToken_COMMA) {
    Match(ConstraintToken_COMMA);
    Match(ConstraintToken_STRING, tmp_value_);
    AtomicValue val = value_set_->Insert(tmp_value_);
    Match(ConstraintToken_COLON);
    Match(ConstraintToken_PROBABILITY, tmp_value_);
    try {
      float prob = boost::lexical_cast<float>(tmp_value_);
      prob_map[val] = prob;
    } catch (const boost::bad_lexical_cast &e) {
      std::ostringstream msg;
      msg << "invalid constraint probability: " << tmp_value_;
      throw Exception(msg.str());
    }
    NTValProbListTail(prob_map);
  }
}

} // namespace internal

ConstraintParser::ConstraintParser(Vocabulary &feature_set,
                                   Vocabulary &value_set)
    : ConstraintParserBase(feature_set, value_set) {
}

boost::shared_ptr<Constraint> ConstraintParser::Parse(const StringPiece &s,
                                                      ConstraintType &type) {
  tokeniser_ = internal::ConstraintTokeniser(s);
  lookahead_ = *tokeniser_;
  // TODO Should empty constraints be allowed?
  if (lookahead_.type == internal::ConstraintToken_EOS) {
    Match(internal::ConstraintToken_EOS);
    return boost::shared_ptr<Constraint>();
  }
  boost::shared_ptr<Constraint> constraint = NTConstraint(type);
  Match(internal::ConstraintToken_EOS);
  return constraint;
}

} // namespace taco
