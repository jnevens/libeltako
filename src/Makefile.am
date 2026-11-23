AM_CPPFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/include

lib_LTLIBRARIES = libeltako.la

libeltako_la_LIBADD =
libeltako_la_LDFLAGS = -version-info $(DC_VERSION_LIBTOOL)

libeltako_la_SOURCES = \
	button.c \
	dimmer.c \
	frame.c \
	frame_receiver.c \
	message.c \
	serial.c \
	version.c

$(libeltako_la_OBJECTS): $(abs_top_builddir)/include/libeltako/version.h
