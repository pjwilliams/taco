AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)
AM_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) \
        $(top_srcdir)/tools-common/libtool-common.la \
        $(top_srcdir)/src/taco/libtaco.la

bin_PROGRAMS = prune-redundant-constraints

prune_redundant_constraints_SOURCES = \
    prune_redundant_constraints.cc \
    prune_redundant_constraints.h \
    main.cc \
    options.h
