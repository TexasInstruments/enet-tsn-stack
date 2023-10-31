#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([tsn_combase], [1.1.3], [shiro@excelfore.com])
AC_SUBST(PACKAGE_DESC,"Excelfore communication base library")

AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AM_PROG_AR
LT_PREREQ([2.2])
LT_INIT([static])
AC_CONFIG_SRCDIR([combase.h])

AS_IF([test -n "$PYTHON_SITEPACKAGES_DIR"],
	[am_cv_python_pythondir="${PYTHON_SITEPACKAGES_DIR}/tsn_combase"],
	[test x"$prefix" = "x/usr"],
	[am_cv_python_pythondir='${PYTHON_PREFIX}/lib/python3/dist-packages/tsn_combase'],
	[am_cv_python_pythondir='${PYTHON_PREFIX}/lib/python${PYTHON_VERSION}/dist-packages/tsn_combase']
	)

AM_PATH_PYTHON([3])

# pkg-config dependency
PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O

# Check if posix platform
AC_CHECK_HEADERS([pthread.h],
	[AM_CONDITIONAL(POSIX_PLATFORM,[true])])

# Check if linux ethtool is available
AC_CHECK_HEADERS([linux/ethtool.h],
	[AM_CONDITIONAL(LINUX_ETHTOOL,[true])])

# Check if linux netlink is available
AC_CHECK_HEADERS([linux/netlink.h],
	[AM_CONDITIONAL(LINUX_NETLINK,[true])])

# Check if linux ethtool is available
AC_CHECK_HEADERS([linux/ethtool_netlink.h],
	[AM_CONDITIONAL(LINUX_ETHTOOL_NETLINK,[true])],
	[AM_CONDITIONAL(LINUX_ETHTOOL_NETLINK,[false])])

# Check for command line options
# Check for command line options
AC_ARG_ENABLE([unittest],
	AS_HELP_STRING([--disable-unittest],[disable building unittest programs]))
AC_ARG_ENABLE([unittest_memtest],
	AS_HELP_STRING([--enable-unittest-memtest],[check memory leak in unittest]))
AC_ARG_ENABLE([ethernet],
	AS_HELP_STRING([--disable-ethernet],[disable building ethernet programs]))
AC_ARG_ENABLE([thread],
	AS_HELP_STRING([--disable-thread],[disable building threadding programs]))
AC_ARG_ENABLE([ipcshmem],
	AS_HELP_STRING([--disable-ipcshmem],[disable building shared memory ipc programs]))
AC_ARG_ENABLE([ipcsock],
	AS_HELP_STRING([--disable-ipcsock],[disable building socket ipc programs]))
AC_ARG_ENABLE([event],
	AS_HELP_STRING([--disable-event],[disable building eventing programs]))
AC_ARG_ENABLE([understpl],
	AS_HELP_STRING([--enable-understpl],[enable understpl mode build]))
AC_ARG_ENABLE([staticmem],
	AS_HELP_STRING([--enable-staticmem],[use static memory allocation]))

# Checks for libraries.
PKG_CHECK_MODULES([x4unibase],[tsn_unibase],,
	[AC_CHECK_HEADERS([tsn_unibase/unibase.h])]
)

# Checks for header files.
AC_CHECK_HEADERS([stdio.h stdlib.h string.h])

if test x"$enable_unittest" != "xno"; then
AC_CHECK_HEADERS([setjmp.h cmocka.h],[],
	[AC_MSG_ERROR([you may need to install libcmocka-dev package])],
	[[#include <string.h>
	#include <stdio.h>
	#include <setjmp.h>
	]])
fi

AM_CONDITIONAL(NO_ETHERNET, [test x"$enable_ethernet" = "xno"])
AM_CONDITIONAL(NO_INET, [test x"$enable_inet" = "xno"])
AM_CONDITIONAL(NO_THREAD, [test x"$enable_thread" = "xno"])
AM_CONDITIONAL(NO_IPCSHMEM, [test x"$enable_ipcshmem" = "xno"])
AM_CONDITIONAL(NO_IPCSOCK, [test x"$enable_ipcsock" = "xno"])
AM_CONDITIONAL(NO_EVENT, [test x"$enable_event" = "xno"])

# conditional build
AM_CONDITIONAL(UNITTEST_MEMTEST, [test x"$enable_unittest_memtest" = "xyes"])
AM_CONDITIONAL(BUILD_IN_STPL, [test x"$enable_understpl" = "xyes"])
AM_CONDITIONAL(STATIC_MEMALLOC, [test x"$enable_staticmem" = "xyes"])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNC([malloc], [], [AC_MSG_ERROR([malloc() function not found.])])
AC_CHECK_FUNC([realloc], [], [AC_MSG_ERROR([realloc() function not found.])])
AC_CHECK_FUNC([memmove], [], [AC_MSG_ERROR([memmove() function not found.])])
AC_CHECK_FUNC([memset], [], [AC_MSG_ERROR([memset() function not found.])])
AC_CHECK_FUNC([strtol], [], [AC_MSG_ERROR([strtol() function not found.])])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([
	Makefile
	tsn_combase.pc])

AC_OUTPUT
