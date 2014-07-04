AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)/tools-common
AM_LDFLAGS = $(BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS)
LDADD = $(BOOST_UNIT_TEST_FRAMEWORK_LIBS) \
        ../libtool-common-m1.la \
        ../../../src/taco/libtaco.la

check_PROGRAMS = test-m1
TESTS = $(check_PROGRAMS)

test_m1_SOURCES = \
    main.cc \
    test_case_model.cc
