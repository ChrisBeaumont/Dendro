#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_LANG_CPLUSPLUS
AC_PREREQ([2.68])
AC_INIT([dendro, 0.1, beaumont@hawaii.edu])
AM_INIT_AUTOMAKE([dendro], [0,1])
AC_CONFIG_SRCDIR([src/])
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lCCFits':
AC_CHECK_LIB([CCfits], [main], [], [
	echo "CCfits library is missing. CCfits is required for this program."
	exit -1])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
