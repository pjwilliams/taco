AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)
AM_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) \
        $(top_srcdir)/tools-common/libtool-common.la \
        $(top_srcdir)/src/taco/libtaco.la

bin_PROGRAMS = add-feature-selection-ids

add_feature_selection_ids_SOURCES = \
    add_feature_selection_ids.cc \
    add_feature_selection_ids.h \
    main.cc \
    options.h
