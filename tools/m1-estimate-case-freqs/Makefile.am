AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)
AM_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) \
        $(top_srcdir)/tools-common/libtool-common.la \
        $(top_srcdir)/src/taco/libtaco.la

bin_PROGRAMS = m1-estimate-case-freqs

m1_estimate_case_freqs_SOURCES = \
    case_inferrer.cc \
    case_inferrer.h \
    m1_estimate_case_freqs.cc \
    m1_estimate_case_freqs.h \
    main.cc \
    options.h \
    tree_parser.h \
    typedef.h
