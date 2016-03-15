AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src
AM_LDFLAGS = $(BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS)
LDADD = $(BOOST_UNIT_TEST_FRAMEWORK_LIBS) \
        $(top_srcdir)/src/taco/libtaco.la

check_PROGRAMS = test-taco
TESTS = $(check_PROGRAMS)

test_taco_SOURCES = \
    main.cc \
    test_constraint.cc \
    test_constraint_evaluator.cc \
    test_constraint_set.cc \
    test_constraint_term.cc \
    test_feature_selection_table.cc \
    test_feature_structure.cc \
    test_interpretation.cc
