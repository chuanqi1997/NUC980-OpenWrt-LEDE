#
# Copyright (C) 2010 segal.di.ubi.pt
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=protocol-collect-relay
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_MAINTAINER:=466005858@qq.com
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/protocol-collect-relay
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Device Collect
endef

define Package/protocol-collect-relay/description
collect device data by uart
endef

#define Package/protocol-collect-relay/conffiles
#/etc/config/system
#endef

define Build/Compile
endef

define Package/protocol-collect-relay/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/root/etc/init.d/protocol-collect-relay.init $(1)/etc/init.d/protocol-collect-relay

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) ./files/root/etc/config/protocol-collect-relay.config $(1)/etc/config/protocol-collect-relay

	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./files/root/usr/sbin/protocol-collect-relay.py $(1)/usr/sbin/protocol-collect-relay.py
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/root/etc/uci-defaults/uci-defaults-protocol-collect-relay $(1)/etc/uci-defaults/50-uci-defaults-protocol-collect-relay
endef

$(eval $(call BuildPackage,protocol-collect-relay))
