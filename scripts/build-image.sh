#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
REPO_ROOT=$(CDPATH= cd -- "${SCRIPT_DIR}/.." && pwd)
BUILDROOT_DIR="${1:-${BUILDROOT_DIR:-}}"

if [ -z "${BUILDROOT_DIR}" ]; then
    echo "usage: $0 /path/to/buildroot" >&2
    exit 1
fi

make BR2_EXTERNAL="${REPO_ROOT}" -C "${BUILDROOT_DIR}" tamagaki_defconfig
make -C "${BUILDROOT_DIR}"
