#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([taco], [0.1], [philip.williams@mac.com])
AM_INIT_AUTOMAKE([foreign])
LT_PREREQ([2.2.6b])
LT_INIT([disable-shared])
AC_CONFIG_SRCDIR([src/taco/feature_structure.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_LANG([C++])

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
#AC_PROG_LN_S
AC_PROG_LIBTOOL

# Checks for libraries.
BOOST_REQUIRE([1.48.0])
# Checks for specific Boost libraries.
BOOST_CONVERSION
BOOST_PROGRAM_OPTIONS
BOOST_SMART_PTR
BOOST_STRING_ALGO
BOOST_TEST
BOOST_UNORDERED
# The following Boost libraries are used in the code but don't seem to be
# supported in boost.m4 yet:
#
# MISSING BOOST CHECK: assign
# MISSING BOOST CHECK: bimap
# MISSING BOOST CHECK: integer

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/taco/Makefile
                 src/taco/base/Makefile
                 src/taco/test/Makefile
                 src/taco/text-formats/Makefile
                 src/taco/text-formats/test/Makefile
                 tools-common/Makefile
                 tools-common/cli/Makefile
                 tools-common/compat-moses/Makefile
                 tools-common/compat-moses/test/Makefile
                 tools-common/compat-nlp-de/Makefile
                 tools-common/compat-nlp-de/test/Makefile
                 tools-common/compat-nlp-el/Makefile
                 tools-common/m1/Makefile
                 tools-common/m1/test/Makefile
                 tools-common/relation/Makefile
                 tools-common/test/Makefile
                 tools-common/text-formats/Makefile
                 tools-common/text-formats/test/Makefile
                 tools/Makefile
                 tools/add-constraint-ids/Makefile
                 tools/add-feature-selection-ids/Makefile
                 tools/combine-constraint-maps/Makefile
                 tools/index-rule-table/Makefile
                 tools/m1-consolidate-constraints/Makefile
                 tools/m1-estimate-case-freqs/Makefile
                 tools/m1-extract-constraints/Makefile
                 tools/m1-extract-lexicon/Makefile
                 tools/m1-extract-vocab/Makefile
                 tools/m1-label-st-sets/Makefile
                 tools/match-constraints-to-rules/Makefile
                 tools/prune-redundant-constraints/Makefile
                 tools/repair-lexicon/Makefile])
AC_OUTPUT
