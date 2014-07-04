#SUBDIRS = test

AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)
AM_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)

noinst_LTLIBRARIES = libtool-lss.la
bin_PROGRAMS = m1-label-st-sets

libtool_lss_la_SOURCES = \
    extractor.h \
    extractor.cc \
    extractor_bitpar.h \
    extractor_bitpar.cc \
    extractor_parzu.h \
    extractor_parzu.cc \
    m1_label_st_sets.cc \
    m1_label_st_sets.h \
    options.h \
    tree_parser.h \
    tree_writer.h \
    typedef.h

libtool_lss_la_LIBS = \
    $(top_srcdir)/tools-common/libtool-common.la \
    $(top_srcdir)/src/taco/libtaco.la \
    $(BOOST_PROGRAM_OPTIONS_LIBS)

m1_label_st_sets_SOURCES = main.cc

m1_label_st_sets_LDADD = \
    libtool-lss.la \
    $(top_srcdir)/tools-common/libtool-common.la \
    $(top_srcdir)/src/taco/libtaco.la \
    $(BOOST_PROGRAM_OPTIONS_LIBS)
