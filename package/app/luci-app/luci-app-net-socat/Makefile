# 
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-net-socat
PKG_VERSION=1.0
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

#PO2LMO:=$(STAGING_DIR_HOST)/bin/po2lmo
PO2LMO:=$(STAGING_DIR_HOSTPKG)/bin/po2lmo
include $(INCLUDE_DIR)/package.mk


define Package/$(PKG_NAME)
SECTION:=luci
CATEGORY:=LuCI
SUBMENU:=3. Applications
TITLE:= LuCI support for NET SOCAT
DEPENDS:=+socat#依赖
PKGARCH:=all
endef

define Package/$(PKG_NAME)/description
 This package contains LuCI configuration pages for net socat.
endef


define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/root/etc/uci-defaults/uci-defaults-net-socat $(1)/etc/uci-defaults/luci-net-socat

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi/net_socat/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/net_socat/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/i18n
#	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/i18n/

	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/tools/


#	$(PO2LMO) ./po/zh-cn/socat.po  ./po/zh-cn/socat.lmo 

#	$(CP)./po/zh-cn/socat.lmo  $(1)$(1)/usr/lib/lua/luci/i18n/socat.lmo

	$(INSTALL_DATA) ./luasrc/controller/net_socat.lua $(1)/usr/lib/lua/luci/controller/net_socat.lua

	$(INSTALL_CONF) ./files/root/etc/config/net_socat $(1)/etc/config/net_socat
	$(INSTALL_BIN) ./files/root/etc/init.d/net_socat $(1)/etc/init.d/net_socat

	$(INSTALL_DATA) ./luasrc/model/cbi/net_socat/tcp-udp-forwards.lua $(1)/usr/lib/lua/luci/model/cbi/net_socat/tcp-udp-forwards.lua
	$(INSTALL_DATA) ./luasrc/model/cbi/net_socat/tcp-udp-forwards-details.lua $(1)/usr/lib/lua/luci/model/cbi/net_socat/tcp-udp-forwards-details.lua
	$(INSTALL_DATA) ./luasrc/view/net_socat/cbi_add_tcp_udp_forward.htm $(1)/usr/lib/lua/luci/view/net_socat/cbi_add_tcp_udp_forward.htm
	$(INSTALL_DATA) ./luasrc/view/net_socat/cbi_show_tcp_udp_forward.htm $(1)/usr/lib/lua/luci/view/net_socat/cbi_show_tcp_udp_forward.htm
	$(INSTALL_DATA) ./luasrc/tools/net_socat.lua $(1)/usr/lib/lua/luci/tools/socat.lua
	$(PO2LMO) ./po/net_socat.zh-cn.po $(1)/usr/lib/lua/luci/i18n/net_socat.zh-cn.lmo
endef

define Package/$(PKG_NAME)/postinst
#!/bin/sh
[ -x /etc/uci-defaults/luci-net-socat ] && /etc/uci-defaults/luci-net-socat || exit 0
endef

define Package/$(PKG_NAME)/postrm
#!/bin/sh
which uci > /dev/null || exit 0
uci -q get ucitrack.@net_socat[0] > /dev/null && {
  uci delete ucitrack.@net_socat[0]
  uci commit
}
exit 0
endef




$(eval $(call BuildPackage,luci-app-net-socat))
