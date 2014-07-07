#SUBDIRS = test

AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)
AM_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)

noinst_LTLIBRARIES = libtool-m3-lss.la
bin_PROGRAMS = m3-label-st-sets

libtool_m3_lss_la_SOURCES = \
    extractor.h \
    extractor.cc \
    m3_label_st_sets.cc \
    m3_label_st_sets.h \
    options.h \
    tree_parser.h \
    tree_writer.h \
    typedef.h

libtool_m3_lss_la_LIBS = \
    $(top_srcdir)/tools-common/libtool-common.la \
    $(top_srcdir)/src/taco/libtaco.la \
    $(BOOST_PROGRAM_OPTIONS_LIBS)

m3_label_st_sets_SOURCES = main.cc

m3_label_st_sets_LDADD = \
    libtool-m3-lss.la \
    $(top_srcdir)/tools-common/libtool-common.la \
    $(top_srcdir)/src/taco/libtaco.la \
    $(BOOST_PROGRAM_OPTIONS_LIBS)
