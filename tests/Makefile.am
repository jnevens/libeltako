AM_CPPFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/include
AM_CFLAGS = $(CHECK_CFLAGS)
LDADD = $(top_builddir)/src/libeltako.la $(CHECK_LIBS)

TESTS = \
	check_frame

@VALGRIND_CHECK_RULES@

check_PROGRAMS = $(TESTS)

check_frame_SOURCES = check_frame.c
