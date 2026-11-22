#!/bin/sh
# Load an OpenSSH private key from GitLab CI variables into a temp file.
# Usage: KEY_FILE="$(./scripts/ci-ssh-key-file.sh PREFIX)"
# PREFIX selects PREFIX_KEY_B64 (recommended) or PREFIX_KEY (File variable path).
set -eu
PREFIX="${1:?prefix required (e.g. APT_PUBLISH or GITHUB_DEPLOY)}"
B64_VAR="${PREFIX}_KEY_B64"
FILE_VAR="${PREFIX}_KEY"
eval "B64=\${${B64_VAR}:-}"
eval "FILE=\${${FILE_VAR}:-}"
KEY_FILE="$(mktemp)"
chmod 600 "${KEY_FILE}"
if [ -n "${B64}" ]; then
	printf '%s' "${B64}" | tr -d ' \n\r' | base64 -d > "${KEY_FILE}"
elif [ -n "${FILE}" ] && [ -f "${FILE}" ]; then
	if grep -q $'\r' "${FILE}" 2>/dev/null; then
		sed 's/\r$//' "${FILE}" > "${KEY_FILE}"
	else
		cp "${FILE}" "${KEY_FILE}"
	fi
	chmod 600 "${KEY_FILE}"
else
	echo "Set ${B64_VAR} (recommended) or ${FILE_VAR} (File)." >&2
	rm -f "${KEY_FILE}"
	exit 1
fi
if [ ! -s "${KEY_FILE}" ]; then
	echo "Deploy key material is empty." >&2
	rm -f "${KEY_FILE}"
	exit 1
fi
PAYLOAD_LINES="$(awk '/BEGIN OPENSSH PRIVATE KEY/{f=1;next} /END OPENSSH PRIVATE KEY/{f=0} f' "${KEY_FILE}" | grep -c . || true)"
if [ "${PAYLOAD_LINES}" -lt 1 ]; then
	echo "No base64 payload between BEGIN and END in ${PREFIX} key." >&2
	rm -f "${KEY_FILE}"
	exit 1
fi
if ! ssh-keygen -y -f "${KEY_FILE}" >/dev/null 2>&1; then
	echo "${PREFIX} key is not a valid unencrypted OpenSSH private key." >&2
	rm -f "${KEY_FILE}"
	exit 1
fi
printf '%s\n' "${KEY_FILE}"
