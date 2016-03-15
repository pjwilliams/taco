SUBDIRS = base test text-formats

AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src

lib_LTLIBRARIES = libtaco.la

nobase_pkginclude_HEADERS = \
    base/basic_types.h \
    base/exception.h \
    base/hash_combine.h \
    base/numbered_set.h \
    base/string_piece.h \
    base/string_util.h \
    base/utility.h \
    base/vocabulary.h \
    constraint.h \
    constraint_evaluator.h \
    constraint_set.h \
    constraint_set_set.h \
    constraint_term.h \
    feature_path.h \
    feature_selection_rule.h \
    feature_selection_table.h \
    feature_structure.h \
    feature_structure_spec.h \
    feature_tree.h \
    interpretation.h \
    lexicon.h \
    option_table.h \
    text-formats/constraint_parser.h \
    text-formats/constraint_set_parser.h \
    text-formats/constraint_set_seq_parser.h \
    text-formats/constraint_set_set_parser.h \
    text-formats/constraint_table_parser.h \
    text-formats/constraint_table_writer.h \
    text-formats/constraint_tokeniser.h \
    text-formats/constraint_writer.h \
    text-formats/feature_structure_parser.h \
    text-formats/feature_structure_writer.h \
    text-formats/feature_structure_seq_parser.h \
    text-formats/feature_structure_tokeniser.h \
    text-formats/feature_tree_parser.h \
    text-formats/lexicon_parser.h

libtaco_la_SOURCES = \
    constraint.cc \
    constraint_evaluator.cc \
    constraint_set.cc \
    constraint_term.cc \
    feature_selection_table.cc \
    feature_structure.cc \
    feature_structure_spec.cc \
    interpretation.cc \
    lexicon.cc \
    option_table.cc

libtaco_la_LIBADD = \
    base/libtaco-base.la \
    text-formats/libtaco-text-formats.la
