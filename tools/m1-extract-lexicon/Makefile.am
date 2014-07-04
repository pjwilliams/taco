AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)
AM_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) \
        $(top_srcdir)/tools-common/libtool-common.la \
        $(top_srcdir)/src/taco/libtaco.la

bin_PROGRAMS = m1-extract-lexicon

m1_extract_lexicon_SOURCES = \
    interpreter.cc \
    interpreter.h \
    m1_extract_lexicon.cc \
    m1_extract_lexicon.h \
    main.cc \
    options.h \
    pos_table.cc \
    pos_table.h
