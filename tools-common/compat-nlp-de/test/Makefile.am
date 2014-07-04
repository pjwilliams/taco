AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)/tools-common
AM_LDFLAGS = $(BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS)
LDADD = $(BOOST_UNIT_TEST_FRAMEWORK_LIBS) \
        ../libtool-common-compat-nlp-de.la \
        ../../../src/taco/libtaco.la

check_PROGRAMS = test-compat-nlp-de
TESTS = $(check_PROGRAMS)

test_compat_nlp_de_SOURCES = \
    main.cc \
    test_bitpar.cc
