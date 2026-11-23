SUBDIRS = include src

if ENABLE_TOOLS
SUBDIRS += tools
endif

if ENABLE_TESTS
SUBDIRS += tests
endif

ACLOCAL_AMFLAGS = -I m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libeltako.pc

EXTRA_DIST = \
	libeltako.pc.in \
	include/libeltako/version.h.in \
	Makefile.am.coverage \
	configure.ac \
	m4/gcov.m4 \
	m4/valgrind.m4 \
	LICENSE \
	README.md \
	misc/cppcheck-env.h \
	misc/cppcheck-suppressions.txt \
	scripts/cppcheck.sh \
	scripts/ci-install-deps.sh \
	scripts/ci-ssh-key-file.sh \
	scripts/publish-apt-reprepro.sh

if HAVE_GCOV
ENABLE_COVERAGE = yes
else
ENABLE_COVERAGE = no
endif

include Makefile.am.coverage

cppcheck:
	@chmod +x scripts/cppcheck.sh
	@./scripts/cppcheck.sh
