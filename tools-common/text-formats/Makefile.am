SUBDIRS = test

AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)/tools-common

noinst_LTLIBRARIES = libtool-common-text-formats.la

libtool_common_text_formats_la_SOURCES = \
    constraint_extract_parser.cc \
    constraint_extract_parser.h \
    constraint_extract_writer.h \
    constraint_map_parser.cc \
    constraint_map_parser.h \
    constraint_map_writer.h \
    rule_table_index_parser.cc \
    rule_table_index_parser.h \
    vocab_parser.cc \
    vocab_parser.h
