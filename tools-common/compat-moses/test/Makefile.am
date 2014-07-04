AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)/tools-common
AM_LDFLAGS = $(BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS)
LDADD = $(BOOST_UNIT_TEST_FRAMEWORK_LIBS) \
        ../libtool-common-compat-moses.la \
        ../../../src/taco/libtaco.la

check_PROGRAMS = test-compat-moses
TESTS = $(check_PROGRAMS)

test_compat_moses_SOURCES = \
    main.cc \
    test_rule_table_parser.cc
