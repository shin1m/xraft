AC_INIT([xraft], [201803])
AM_INIT_AUTOMAKE([foreign nostdinc dist-bzip2 no-dist-gzip])
AC_CONFIG_SRCDIR([src/application_x11.cc])

PKG_CHECK_MODULES([X11], [x11 >= 1.4])
PKG_CHECK_MODULES([DBUS], [dbus-1 >= 1.4])
PKG_CHECK_MODULES([GL], [gl >= 7.11])
PKG_CHECK_MODULES([CAIRO], [cairo >= 1.8])
XEMMAI_USE=xemmai-201803
PKG_CHECK_MODULES([XEMMAI], [${XEMMAI_USE}], [
	XEMMAI_MODULE_PATH=`pkg-config --variable=module_path ${XEMMAI_USE}`
])
AC_SUBST(XEMMAI_MODULE_PATH)

if test "${CXXFLAGS+set}" != set; then
	CXXFLAGS=
fi
AC_PROG_CXX
AM_DISABLE_STATIC
AM_PROG_LIBTOOL

AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

AC_ARG_ENABLE(
	[debug],
	AS_HELP_STRING([--enable-debug], [turn on debugging]),
	[case "${enableval}" in
	  yes) debug=true ;;
	  no) debug=false ;;
	  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
	 esac],
	[debug=false]
)
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])
AC_ARG_ENABLE(
	[profile],
	AS_HELP_STRING([--enable-profile], [turn on profiling]),
	[case "${enableval}" in
	  yes) profile=true ;;
	  no) profile=false ;;
	  *) AC_MSG_ERROR([bad value ${enableval} for --enable-profile]) ;;
	 esac],
	[profile=false]
)
AM_CONDITIONAL([PROFILE], [test x$profile = xtrue])

AC_CONFIG_HEADERS([include/xraft/configure.h])
AC_CONFIG_FILES([
	Makefile
	include/Makefile
	src/Makefile
	xraftvt/Makefile
	xraftwm/Makefile
	xraftim/Makefile
	xrafttv/Makefile
	xemmai/Makefile
	examples/Makefile
])
AC_OUTPUT
