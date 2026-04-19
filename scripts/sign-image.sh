#!/bin/sh
set -eu

if [ "$#" -ne 2 ]; then
    echo "usage: $0 /path/to/image /path/to/private-key.pem" >&2
    exit 1
fi

IMAGE="$1"
KEY="$2"
DIR=$(dirname "${IMAGE}")
FILE=$(basename "${IMAGE}")

openssl dgst -sha256 -sign "${KEY}" -out "${IMAGE}.sig" "${IMAGE}"
(
    cd "${DIR}"
    sha256sum "${FILE}" > "${FILE}.sha256"
)
