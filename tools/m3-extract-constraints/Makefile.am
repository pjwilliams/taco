AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)
AM_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) \
        $(top_srcdir)/tools-common/libtool-common.la \
        $(top_srcdir)/src/taco/libtaco.la

bin_PROGRAMS = m3-extract-constraints

m3_extract_constraints_SOURCES = \
    extractor.cc \
    extractor.h \
    m3_extract_constraints.cc \
    m3_extract_constraints.h \
    main.cc \
    options.h \
    tree_context.cc \
    tree_context.h \
    tree_fragment.cc \
    tree_fragment.h \
    tree_parser.h \
    typedef.h \
    writer.cc \
    writer.h
