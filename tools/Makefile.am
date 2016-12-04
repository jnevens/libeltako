AM_CPPFLAGS = -I$(top_buildir)/include -I$(top_srcdir)/include
LDADD = $(top_builddir)/src/.libs/libeltako.la

sbin_PROGRAMS = eltako-dump eltako-send eltakod

eltako_dump_SOURCES = \
	dump.c

eltako_send_SOURCES = \
	send.c

eltakod_LDADD = -lvsb -levquick $(LDADD)
eltakod_SOURCES = \
	eltakod.c
