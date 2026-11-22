#!/bin/sh
# Install libvsb and the bundled libevquick so eltakod can be built.
# CI installs into /usr. A local prefix works too: ELTAKO_DEPS_PREFIX=/opt/deps
set -eu
PREFIX="${ELTAKO_DEPS_PREFIX:-/usr}"

if pkg-config --exists libvsb; then
	libdir=$(pkg-config --variable=libdir libvsb)
	if [ -f "${libdir}/libvsb.a" ] && [ -f "${libdir}/libevquick.a" ]; then
		echo "libvsb and libevquick already installed in ${libdir}"
		exit 0
	fi
fi

workdir=$(mktemp -d)
trap 'rm -rf "${workdir}"' EXIT
git clone --depth 1 https://github.com/jnevens/libvsb.git "${workdir}/libvsb"
cd "${workdir}/libvsb"
autoreconf -fi
./configure \
	--prefix="${PREFIX}" \
	--disable-tests \
	--disable-valgrind \
	--disable-vsbtools \
	--disable-examples
make -j"$(nproc)"
make install
if [ "${PREFIX}" = /usr ] || [ "${PREFIX}" = /usr/local ]; then
	ldconfig || true
fi
echo "Installed libvsb and libevquick into ${PREFIX}"
