#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([usage], [1.0], [reri0702@gmail.com])
AM_INIT_AUTOMAKE([-Wall gnu foreign])
AC_CONFIG_SRCDIR([src/cpucalc.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_RANLIB
LT_INIT([dlopen])

# Checks for programs.
AC_PROG_CC
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Check function
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strstr])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([strcasecmp])
AC_CHECK_FUNCS([sysinfo])
AC_CHECK_FUNCS([strspn])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])
AC_CHECK_HEADERS([sys/statvfs.h])
AC_CHECK_HEADERS([sys/vfs.h])
AC_CHECK_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

AC_ARG_ENABLE([test],
    AS_HELP_STRING([--with-test],
                   [Enable debug mode (_DEBUG macro)]),
    [AC_DEFINE([_DEBUG], [1], [Define to enable debug mode])],
    [enableval=no])

AC_CONFIG_FILES([Makefile
                  src/Makefile
                  test/Makefile])
AC_OUTPUT
