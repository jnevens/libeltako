AM_CPPFLAGS = -I$(top_srcdir)/include
AM_CFLAGS = -pthread -lcheck_pic -lrt -lm
LDADD = ../src/libeltako.la

TESTS = 
@VALGRIND_CHECK_RULES@

check_PROGRAMS = $(TESTS)

# check_eltako_SOURCES = check_eltako.c
