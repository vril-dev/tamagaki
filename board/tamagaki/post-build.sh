#!/bin/sh
set -eu

TARGET_DIR="${1:?missing TARGET_DIR}"

mkdir -p "${TARGET_DIR}/config"
mkdir -p "${TARGET_DIR}/var/lib/tamagaki"
chmod 0755 "${TARGET_DIR}/etc/init.d/S99tamagaki"
