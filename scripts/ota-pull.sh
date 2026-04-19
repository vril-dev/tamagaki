#!/bin/sh
set -eu

OTA_SERVER="${OTA_SERVER:-https://ota.example.com}"
PUBKEY="${PUBKEY:-/etc/tamagaki/ota.pub}"
TMP_IMG="${TMP_IMG:-/tmp/new.img}"
TMP_SIG="${TMP_SIG:-/tmp/new.img.sig}"
TMP_SHA="${TMP_SHA:-/tmp/new.img.sha256}"

CURRENT_PART=$(fw_printenv boot_partition | cut -d= -f2)
if [ "${CURRENT_PART}" = "B" ]; then
    TARGET_PART="/dev/mmcblk0p3"
    NEXT_PART="C"
else
    TARGET_PART="/dev/mmcblk0p2"
    NEXT_PART="B"
fi

LATEST=$(curl -sf "${OTA_SERVER}/latest")
CURRENT=$(cat /etc/tamagaki/version)

if [ "${LATEST}" = "${CURRENT}" ]; then
    exit 0
fi

curl -sf "${OTA_SERVER}/${LATEST}.img" -o "${TMP_IMG}"
curl -sf "${OTA_SERVER}/${LATEST}.img.sig" -o "${TMP_SIG}"
curl -sf "${OTA_SERVER}/${LATEST}.sha256" -o "${TMP_SHA}"

cd /tmp
sha256sum -c "$(basename "${TMP_SHA}")"
openssl dgst -sha256 -verify "${PUBKEY}" -signature "${TMP_SIG}" "${TMP_IMG}"

IMG_SIZE=$(stat -c%s "${TMP_IMG}")
if [ "${IMG_SIZE}" -lt 10485760 ]; then
    echo "ERROR: image too small: ${IMG_SIZE} bytes" >&2
    exit 1
fi

dd if="${TMP_IMG}" of="${TARGET_PART}" bs=4M conv=fsync
sync

fw_setenv fallback_partition "${CURRENT_PART}"
fw_setenv boot_partition "${NEXT_PART}"
fw_setenv upgrade_available 1

reboot
