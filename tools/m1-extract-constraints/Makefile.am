AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)
AM_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) \
        $(top_srcdir)/tools-common/libtool-common.la \
        $(top_srcdir)/src/taco/libtaco.la

bin_PROGRAMS = m1-extract-constraints

m1_extract_constraints_SOURCES = \
    extractor.cc \
    extractor.h \
    m1_extract_constraints.cc \
    m1_extract_constraints.h \
    main.cc \
    options.h \
    tree_context.cc \
    tree_context.h \
    tree_context_bitpar.cc \
    tree_context_bitpar.h \
    tree_context_parzu.cc \
    tree_context_parzu.h \
    tree_fragment.cc \
    tree_fragment.h \
    tree_parser.h \
    typedef.h \
    writer.cc \
    writer.h
