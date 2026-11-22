# Release version (also bump debian/changelog on release).
m4_define([dc_version_major],[0])
m4_define([dc_version_minor],[1])
m4_define([dc_version_micro],[1])
m4_define([dc_version_suffix],[devel])
m4_define([dc_version],dc_version_major.dc_version_minor.dc_version_micro[]m4_ifset([dc_version_suffix],-[dc_version_suffix]))
# Libtool versioning (ABI: current:revision:age). SONAME is libeltako.so.0.
m4_define([dc_version_lt_current],[0])
m4_define([dc_version_lt_revision],[0])
m4_define([dc_version_lt_age],[0])

AC_PREREQ([2.69])
AC_INIT([libeltako],[dc_version])
AC_CONFIG_SRCDIR([src/frame.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_PREFIX_DEFAULT([/usr])

AM_INIT_AUTOMAKE([-Wall -Werror -Wno-portability foreign])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

LT_PREREQ([2.2.0])
LT_INIT()
LT_PROG_RC

AC_PROG_CC
PKG_PROG_PKG_CONFIG

# Tool applications.
AC_ARG_ENABLE([tools],
	[AS_HELP_STRING([--enable-tools=@<:@yes/no@:>@],
		[Build eltako-dump, eltako-send, and eltakod @<:@default=yes@:>@])],
	[], [enable_tools=yes])
AM_CONDITIONAL([ENABLE_TOOLS], [test "x$enable_tools" = "xyes"])

# eltakod talks to libvsb and libevquick. Prefer the static archives so the
# Debian package does not depend on shared libraries that are not in Ubuntu
# or the internal apt repository. libvsb's build installs both archives.
AC_ARG_ENABLE([eltakod],
	[AS_HELP_STRING([--enable-eltakod=@<:@yes/no/auto@:>@],
		[Build eltakod @<:@default=auto@:>@])],
	[], [enable_eltakod=auto])

have_libvsb=no
have_libevquick=no
ELTAKOD_CFLAGS=
ELTAKOD_LIBS=
AS_IF([test "x$enable_eltakod" != xno], [
	PKG_CHECK_MODULES([LIBVSB], [libvsb], [have_libvsb=yes], [have_libvsb=no])
	AS_IF([test "x$have_libvsb" = xyes], [
		vsb_libdir=`$PKG_CONFIG --variable=libdir libvsb`
		vsb_cflags=`$PKG_CONFIG --cflags libvsb`
		AS_IF([test -f "$vsb_libdir/libvsb.a" && test -f "$vsb_libdir/libevquick.a"], [
			have_libevquick=yes
			ELTAKOD_CFLAGS=$vsb_cflags
			ELTAKOD_LIBS="$vsb_libdir/libvsb.a $vsb_libdir/libevquick.a"
		], [
			eltako_save_CPPFLAGS=$CPPFLAGS
			CPPFLAGS="$vsb_cflags $CPPFLAGS"
			AC_CHECK_HEADER([libevquick/libevquick.h],
				[have_libevquick=yes], [have_libevquick=no])
			CPPFLAGS=$eltako_save_CPPFLAGS
			AS_IF([test "x$have_libevquick" = xyes], [
				ELTAKOD_CFLAGS=$vsb_cflags
				ELTAKOD_LIBS="$LIBVSB_LIBS -levquick"
			])
		])
	])
	AS_IF([test "x$have_libvsb" = xyes && test "x$have_libevquick" = xyes],
		[enable_eltakod=yes],
		[AS_IF([test "x$enable_eltakod" = xyes],
			[AC_MSG_ERROR([eltakod needs libvsb and libevquick])],
			[enable_eltakod=no
			 AC_MSG_WARN([libvsb or libevquick not found; eltakod will not be built])])])
])
AM_CONDITIONAL([ENABLE_ELTAKOD], [test "x$enable_eltakod" = "xyes"])
AC_SUBST([ELTAKOD_CFLAGS])
AC_SUBST([ELTAKOD_LIBS])

# Tests. Accept "y" as well as "yes" (--enable-tests=y).
AC_ARG_ENABLE([tests],
	[AS_HELP_STRING([--enable-tests=@<:@yes/no@:>@],
		[Build self tests @<:@default=no@:>@])],
	[], [enable_tests=no])
AS_CASE([$enable_tests],
	[yes|y], [enable_tests=yes],
	[enable_tests=no])

AX_VALGRIND_CHECK

m4_include([m4/gcov.m4])
AC_TDD_GCOV
AC_SUBST(COVERAGE_CFLAGS)
AC_SUBST(COVERAGE_CXXFLAGS)
AC_SUBST(COVERAGE_LDFLAGS)

# Coverage replays the test suite, so turn tests on with gcov.
AS_IF([test "x$use_gcov" = "xyes" && test "x$enable_tests" != "xyes"],
	[enable_tests=yes])
AM_CONDITIONAL([ENABLE_TESTS], [test "x$enable_tests" = "xyes"])
AS_IF([test "x$enable_tests" = "xyes"], [
	PKG_CHECK_MODULES([CHECK], [check])
	# libcheck is a static archive that needs libsubunit. Link the
	# subunit archive itself so the tests do not depend on
	# libsubunit.so.0 being on the default runtime path.
	subunit_libdir=`$PKG_CONFIG --variable=libdir libsubunit`
	AS_IF([test -n "$subunit_libdir" && test -f "$subunit_libdir/libsubunit.a"], [
		CHECK_LIBS=`echo "$CHECK_LIBS" | sed "s|-lsubunit|$subunit_libdir/libsubunit.a|"`
	])
])

AC_SUBST([DC_VERSION],[dc_version])
AC_SUBST([DC_VERSION_MAJOR],[dc_version_major])
AC_SUBST([DC_VERSION_MINOR],[dc_version_minor])
AC_SUBST([DC_VERSION_MICRO],[dc_version_micro])
AC_SUBST([DC_VERSION_LIBTOOL],[dc_version_lt_current:dc_version_lt_revision:dc_version_lt_age])

m4_ifset([dc_version_suffix],[
	AC_DEFINE([HAVE_VERSION_SUFFIX], [1], [Define if a version suffix is present.])
])

AC_CONFIG_FILES([
	libeltako.pc
	Makefile
	include/Makefile
	include/libeltako/Makefile
	src/Makefile
	tools/Makefile
	tests/Makefile
])

AC_OUTPUT
