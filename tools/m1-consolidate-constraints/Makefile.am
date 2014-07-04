AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)
AM_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) \
        $(top_srcdir)/tools-common/libtool-common.la \
        $(top_srcdir)/src/taco/libtaco.la

bin_PROGRAMS = m1-consolidate-constraints

m1_consolidate_constraints_SOURCES = \
    batch.cc \
    batch.h \
    consolidator.cc \
    consolidator.h \
    cs_merger.cc \
    cs_merger.h \
    m1_consolidate_constraints.cc \
    m1_consolidate_constraints.h \
    main.cc \
    options.h \
    statistics.h \
    typedef.h
