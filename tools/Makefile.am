AM_CPPFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/include
LDADD = $(top_builddir)/src/libeltako.la

sbin_PROGRAMS = eltako-dump eltako-send

eltako_dump_SOURCES = \
	dump.c

eltako_send_SOURCES = \
	send.c

if ENABLE_ELTAKOD
sbin_PROGRAMS += eltakod

eltakod_CPPFLAGS = $(AM_CPPFLAGS) $(ELTAKOD_CFLAGS)
eltakod_LDADD = $(ELTAKOD_LIBS) $(LDADD)
eltakod_SOURCES = \
	eltakod.c config.c

eltakodir = $(sysconfdir)/eltako
eltako_DATA = eltakod.conf

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(eltakodir)/scripts.d

uninstall-hook:
	-rmdir $(DESTDIR)$(eltakodir)/scripts.d
	-rmdir $(DESTDIR)$(eltakodir)
else
install-data-local:
	@:

uninstall-hook:
	@:
endif

noinst_HEADERS = \
	config.h

EXTRA_DIST = \
	eltakod.conf
