AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)
AM_LDFLAGS = $(BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS)
LDADD = $(BOOST_UNIT_TEST_FRAMEWORK_LIBS) \
        $(top_srcdir)/tools-common/libtool-common.la \
        $(top_srcdir)/src/taco/libtaco.la

check_PROGRAMS = test-tools-common
TESTS = $(check_PROGRAMS)

test_tools_common_SOURCES = \
    main.cc \
    test_constraint_table.cc
