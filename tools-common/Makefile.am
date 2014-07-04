SUBDIRS = cli \
          compat-moses \
          compat-nlp-de \
          m1 \
          relation \
          test \
          text-formats

AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src

noinst_LTLIBRARIES = libtool-common.la

libtool_common_la_SOURCES = \
    constraint_table.cc \
    constraint_table.h \
    feature_selection_map.cc \
    feature_selection_map.h

libtool_common_la_LIBADD = \
    cli/libtool-common-cli.la \
    compat-moses/libtool-common-compat-moses.la \
    compat-nlp-de/libtool-common-compat-nlp-de.la \
    m1/libtool-common-m1.la \
    relation/libtool-common-relation.la \
    text-formats/libtool-common-text-formats.la
