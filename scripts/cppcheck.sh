#!/bin/sh
# Run cppcheck with the same flags as GitLab CI. Requires ./configure first.
# syntaxError/constVariablePointer are suppressed: glibc + _GNU_SOURCE confuses cppcheck.
set -eu

root=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
cd "$root"

if ! test -f config.log; then
	echo "Run ./configure in $root before scripts/cppcheck.sh" >&2
	exit 1
fi

CPPCHECK_STD=c17
if grep -qE 'using -std=(gnu23|c23)' config.log; then
	CPPCHECK_STD=c23
elif grep -qE 'using -std=(gnu2x|c2x)' config.log; then
	CPPCHECK_STD=c2x
fi

vsb_cflags=
if pkg-config --exists libvsb; then
	vsb_cflags=$(pkg-config --cflags libvsb)
fi

echo "cppcheck --std=${CPPCHECK_STD}"

exec cppcheck \
	--std="${CPPCHECK_STD}" \
	--enable=warning,style,performance,portability \
	--error-exitcode=1 \
	--inline-suppr \
	--suppress=missingIncludeSystem \
	--suppress=syntaxError \
	--suppress=constVariablePointer \
	--include=misc/cppcheck-env.h \
	--suppressions-list=misc/cppcheck-suppressions.txt \
	-I include \
	-I src \
	-I tools \
	${vsb_cflags} \
	src/*.c \
	tools/*.c \
	tests/*.c \
	"$@"
