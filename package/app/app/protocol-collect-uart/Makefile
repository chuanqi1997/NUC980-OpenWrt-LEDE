#
# Copyright (C) 2010 segal.di.ubi.pt
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=protocol-collect-uart
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_MAINTAINER:=466005858@qq.com
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/protocol-collect-uart
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Device Collect
endef

define Package/protocol-collect-uart/description
collect device data by uart
endef

#define Package/protocol-collect-uart/conffiles
#/etc/config/system
#endef

define Build/Compile
endef

define Package/protocol-collect-uart/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/root/etc/init.d/protocol-collect-uart.init $(1)/etc/init.d/protocol-collect-uart

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) ./files/root/etc/config/protocol-collect-uart.config $(1)/etc/config/protocol-collect-uart

	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./files/root/usr/sbin/protocol-collect-uart.py $(1)/usr/sbin/protocol-collect-uart.py
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/root/etc/uci-defaults/uci-defaults-protocol-collect-uart $(1)/etc/uci-defaults/50-uci-defaults-protocol-collect-uart
endef

$(eval $(call BuildPackage,protocol-collect-uart))
