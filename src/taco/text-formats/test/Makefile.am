AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src
AM_LDFLAGS = $(BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS)
LDADD = $(BOOST_UNIT_TEST_FRAMEWORK_LIBS) \
        $(top_srcdir)/src/taco/libtaco.la

check_PROGRAMS = test-taco-text-formats
TESTS = $(check_PROGRAMS)

test_taco_text_formats_SOURCES = \
    main.cc \
    test_constraint_parser.cc \
    test_constraint_set_parser.cc \
    test_constraint_set_seq_parser.cc \
    test_constraint_set_set_parser.cc \
    test_constraint_tokeniser.cc \
    test_feature_structure_parser.cc \
    test_feature_structure_seq_parser.cc \
    test_feature_structure_tokeniser.cc \
    test_feature_structure_writer.cc \
    test_feature_tree_parser.cc \
    test_lexicon_parser.cc
