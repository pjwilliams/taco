AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)/tools-common
AM_LDFLAGS = $(BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS)
LDADD = $(BOOST_UNIT_TEST_FRAMEWORK_LIBS) \
        $(top_srcdir)/tools-common/text-formats/libtool-common-text-formats.la \
        $(top_srcdir)/src/taco/libtaco.la

check_PROGRAMS = test-tools-common-text-formats
TESTS = $(check_PROGRAMS)

test_tools_common_text_formats_SOURCES = \
    main.cc \
    test_constraint_extract_parser.cc \
    test_constraint_map_parser.cc \
    test_vocab_parser.cc
