#
# Copyright (C) 2010 segal.di.ubi.pt
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ztk_service
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_MAINTAINER:=466005858@qq.com
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/ztk_service
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=ZTK Access control Service
endef

define Package/ztk_service/description
ZTK Access control HTTP Server
endef

#define Package/ztk_service/conffiles
#/etc/config/system
#endef

define Build/Compile
endef

define Package/ztk_service/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/root/etc/init.d/ztk_service.init $(1)/etc/init.d/ztk_service

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) ./files/root/etc/config/ztk_service.config $(1)/etc/config/ztk_service

	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./files/root/usr/sbin/ztk_service.py $(1)/usr/sbin/ztk_service.py
	
	#$(INSTALL_DIR) $(1)/etc/uci-defaults
	#$(INSTALL_BIN) ./files/root/etc/uci-defaults/uci-defaults-ztk_service $(1)/etc/uci-defaults/50-uci-defaults-ztk_service

endef

$(eval $(call BuildPackage,ztk_service))
