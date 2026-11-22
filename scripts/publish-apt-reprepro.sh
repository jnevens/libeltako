#!/bin/sh
# Upload .deb files to reprepro incoming/, then includedeb (CI or manual).
set -eu
DEB_DIR="${1:-deb}"
SSH_TARGET="${APT_PUBLISH_SSH:-reprepro@192.168.64.110}"
REPREPRO_BASE="${APT_REPREPRO_BASE:-/srv/apt}"
INCOMING="${APT_INCOMING_DIR:-${REPREPRO_BASE}/incoming}"
SUITE="${APT_REPREPRO_SUITE:?set APT_REPREPRO_SUITE (reprepro distribution, e.g. noble)}"
case "${REPREPRO_BASE}" in
/*) ;;
*)
	echo "APT_REPREPRO_BASE must be an absolute path." >&2
	exit 1
	;;
esac
case "${INCOMING}" in
/*) ;;
*)
	echo "APT_INCOMING_DIR must be an absolute path." >&2
	exit 1
	;;
esac
case "${SUITE}" in
*[!a-zA-Z0-9._+-]*)
	echo "APT_REPREPRO_SUITE contains invalid characters." >&2
	exit 1
	;;
esac
if [ ! -d "${DEB_DIR}" ] || ! ls "${DEB_DIR}"/*.deb >/dev/null 2>&1; then
	echo "No .deb files in ${DEB_DIR}/" >&2
	exit 1
fi
STAGING="${INCOMING}/libeltako-${CI_PIPELINE_ID:-manual}-$$"
ssh -o BatchMode=yes "${SSH_TARGET}" "mkdir -p '${STAGING}'"
scp -o BatchMode=yes "${DEB_DIR}"/*.deb "${SSH_TARGET}:${STAGING}/"
ssh -o BatchMode=yes "${SSH_TARGET}" "
set -eu
for f in '${STAGING}'/*.deb; do
	reprepro -Vb '${REPREPRO_BASE}' includedeb '${SUITE}' \"\${f}\"
done
rm -rf '${STAGING}'
"
echo "Published $(ls "${DEB_DIR}"/*.deb | wc -l) package(s) to ${SUITE} via ${INCOMING} on ${SSH_TARGET}"
