AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)
AM_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) \
        $(top_srcdir)/tools-common/libtool-common.la \
        $(top_srcdir)/src/taco/libtaco.la

bin_PROGRAMS = combine-constraint-maps

combine_constraint_maps_SOURCES = \
    combine_constraint_maps.cc \
    combine_constraint_maps.h \
    input_reader.cc \
    input_reader.h \
    main.cc \
    options.h
