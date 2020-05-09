# 
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-ztk_service
PKG_VERSION=0.1
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

#PO2LMO:=$(STAGING_DIR_HOST)/bin/po2lmo
PO2LMO:=$(STAGING_DIR_HOSTPKG)/bin/po2lmo
include $(INCLUDE_DIR)/package.mk


define Package/$(PKG_NAME)
SECTION:=luci
CATEGORY:=LuCI
SUBMENU:=3. Applications
TITLE:= LuCI support for ZTK Access Service Config
#DEPENDS:=+socat#依赖
PKGARCH:=all
endef

define Package/$(PKG_NAME)/description
 This package contains LuCI configuration pages for ZTK Service Config
endef

define Build/Prepare
endef

define Build/Configure
endef


define Build/Compile
endef


define Package/$(PKG_NAME)/install
	
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/root/etc/uci-defaults/uci-defaults-ztk_service $(1)/etc/uci-defaults/luci-app-ztk_service

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi/ztk_service/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/ztk_service/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/i18n
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/tools/


	$(INSTALL_DATA) ./luasrc/controller/ztk_service.lua $(1)/usr/lib/lua/luci/controller/ztk_service.lua

#	$(INSTALL_CONF) ./files/root/etc/config/ztk_service $(1)/etc/config/ztk_service
#	$(INSTALL_BIN) ./files/root/etc/init.d/ztk_service $(1)/etc/init.d/ztk_service

	#$(INSTALL_DATA) ./luasrc/model/cbi/ztk_service/tcp-udp-forwards.lua $(1)/usr/lib/lua/luci/model/cbi/ztk_service/tcp-udp-forwards.lua


	$(INSTALL_DATA) ./luasrc/model/cbi/ztk_service/ztk_service.lua  $(1)/usr/lib/lua/luci/model/cbi/ztk_service/ztk_service.lua
	$(INSTALL_DATA) ./luasrc/model/cbi/ztk_service/run_status.lua  $(1)/usr/lib/lua/luci/model/cbi/ztk_service/run_status.lua

	#$(INSTALL_DATA) ./luasrc/model/cbi/ztk_service/ztk_service_log.lua  $(1)/usr/lib/lua/luci/model/cbi/ztk_service/ztk_service_log.lua

	#$(INSTALL_DATA) ./luasrc/model/cbi/ztk_service/collect.lua  $(1)/usr/lib/lua/luci/model/cbi/ztk_service/collect.lua
	#$(INSTALL_DATA) ./luasrc/model/cbi/ztk_service/forward.lua  $(1)/usr/lib/lua/luci/model/cbi/ztk_service/forward.lua

	#$(INSTALL_DATA) ./luasrc/model/cbi/ztk_service/collect-details.lua  $(1)/usr/lib/lua/luci/model/cbi/ztk_service/collect-details.lua
	#$(INSTALL_DATA) ./luasrc/model/cbi/ztk_service/forward-details.lua  $(1)/usr/lib/lua/luci/model/cbi/ztk_service/forward-details.lua


	$(INSTALL_DATA) ./luasrc/view/ztk_service/run_status.htm $(1)/usr/lib/lua/luci/view/ztk_service/run_status.htm

	#$(INSTALL_DATA) ./luasrc/view/ztk_service/cbi_show_tcp_udp_forward.htm $(1)/usr/lib/lua/luci/view/ztk_service/cbi_show_tcp_udp_forward.htm
	
	#$(INSTALL_DATA) ./luasrc/tools/ztk_service.lua $(1)/usr/lib/lua/luci/tools/ztk_service.lua

	$(PO2LMO) ./po/ztk_service.zh-cn.po $(1)/usr/lib/lua/luci/i18n/ztk_service.zh-cn.lmo

endef

define Package/$(PKG_NAME)/postinst
#!/bin/sh
[ -x /etc/uci-defaults/$(PKG_NAME) ] && /etc/uci-defaults/$(PKG_NAME) || exit 0
endef

define Package/$(PKG_NAME)/postrm
#!/bin/sh
which uci > /dev/null || exit 0
uci -q get ucitrack.@ztk_service[0] > /dev/null && {
  uci delete ucitrack.@ztk_service[0]
  uci commit
}
exit 0
endef


$(eval $(call BuildPackage,$(PKG_NAME)))
