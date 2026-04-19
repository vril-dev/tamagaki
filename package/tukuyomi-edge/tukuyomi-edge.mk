################################################################################
#
# tukuyomi-edge
#
################################################################################

TUKUYOMI_EDGE_VERSION = 0.12.12
TUKUYOMI_EDGE_SITE = https://github.com/vril-dev/tukuyomi-releases/releases/download/v$(TUKUYOMI_EDGE_VERSION)
TUKUYOMI_EDGE_SOURCE = tukuyomi-edge-v$(TUKUYOMI_EDGE_VERSION)-linux-$(if $(BR2_aarch64),arm64,$(if $(BR2_arm),armv7,amd64)).tar.gz

define TUKUYOMI_EDGE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/tukuyomi-edge $(TARGET_DIR)/usr/bin/tukuyomi-edge
endef

$(eval $(generic-package))
