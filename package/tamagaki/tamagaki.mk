################################################################################
#
# tamagaki
#
################################################################################

TAMAGAKI_VERSION = local
TAMAGAKI_SITE = $(BR2_EXTERNAL_TAMAGAKI_PATH)
TAMAGAKI_SITE_METHOD = local
TAMAGAKI_SUPPORTS_IN_SOURCE_BUILD = NO

TAMAGAKI_DEPENDENCIES = libbpf host-pkgconf
TAMAGAKI_CONF_OPTS += -DTAMAGAKI_STRICT_LIBBPF=ON

define TAMAGAKI_USERS
	tamagaki -1 tamagaki -1 * /var/lib/tamagaki - - Tamagaki runtime user
endef

define TAMAGAKI_PERMISSIONS
	/usr/libexec/tamagaki-notify-boot f 4755 root root - - - - -
endef

$(eval $(cmake-package))
