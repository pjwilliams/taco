AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)
AM_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) \
        $(top_srcdir)/tools-common/libtool-common.la \
        $(top_srcdir)/src/taco/libtaco.la

bin_PROGRAMS = match-constraints-to-rules

match_constraints_to_rules_SOURCES = \
    match_constraints_to_rules.cc \
    match_constraints_to_rules.h \
    main.cc \
    options.h
