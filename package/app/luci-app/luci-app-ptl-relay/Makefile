# 
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-ptl-relay
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
TITLE:= LuCI support for Protocol Relay
#DEPENDS:=+socat#依赖
PKGARCH:=all
endef

define Package/$(PKG_NAME)/description
 This package contains LuCI configuration pages for Ptl Relay
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
#	$(CP) ./src/lib60870-C/* $(PKG_BUILD_DIR)
#	$(CP) ./src/libiec61850-1.4.0/* $(PKG_BUILD_DIR)
endef

define Build/Configure

endef

#TARGET_CFLAGS += $(FPIC)
#define Build/Compile
#	$(MAKE) -C $(PKG_BUILD_DIR) \
#		TARGET = "LINUX-MIPSEL"
#		CFLAGS="$(TARGET_CFLAGS)" \ 
#		$(TARGET_CONFIGURE_OPTS)# \
#		CFLAGS="$(TARGET_CFLAGS) -I$(LINUX_DIR)/include" \
#		dynlib
#endef
#Build/Compile (optional)
#How to compile the source; in most cases you should leave this undefined, because then the default is used, which calls make. If you want #to pass special arguments to make, use e.g. “$(call Build/Compile/Default,FOO=bar)”


define Build/Compile
#	$(call Build/Compile/Default,dynlib)
#	$(MAKE) -C $(PKG_BUILD_DIR) \
#		$(TARGET_CONFIGURE_OPTS) \
#		CFLAGS="$(TARGET_CFLAGS) -I$(LINUX_DIR)/include" \
#		CPPFLAGS="$(TARGET_CPPFLAGS)" \ 
#		LDFLAGS="$(TARGET_LDFLAGS)" \
#		dynlib
#		$(dynlib)
endef

#define Build/Compile
#默认是编译源码里面的Makefile，如果你想传递一些参数比如环
#境变量什么的，那就可以定义，编译方法，没有特别说明的可以不予以定义。如果不
#定义将使用默认的编译方法Build/Compile/Default
#自行开发的软件包可以考虑使用下面的定义。
#
#	$(MAKE) -C  $(PKG_BUILD_DIR)\
#	$(TARGET_CONFIGURE_OPTS)\
#	CFLAGS="$(TARGET_CFLAGS)-I$(LINUX_DIR)/include"
#endef

#define Build/Compile
#endef

define Package/$(PKG_NAME)/install
	
	$(INSTALL_DIR) $(1)/etc/uci-defaults
#	$(INSTALL_BIN) ./files/root/etc/uci-defaults/uci-defaults-net-socat $(1)/etc/uci-defaults/luci-net-socat

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi/ptl-relay/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/ptl-relay/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/i18n
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/tools/


	$(INSTALL_DATA) ./luasrc/controller/ptl-relay.lua $(1)/usr/lib/lua/luci/controller/ptl-relay.lua

#	$(INSTALL_CONF) ./files/root/etc/config/ptl-relay $(1)/etc/config/ptl-relay
#	$(INSTALL_BIN) ./files/root/etc/init.d/ptl-relay $(1)/etc/init.d/ptl-relay

	$(INSTALL_DATA) ./luasrc/model/cbi/ptl-relay/tcp-udp-forwards.lua $(1)/usr/lib/lua/luci/model/cbi/ptl-relay/tcp-udp-forwards.lua


	$(INSTALL_DATA) ./luasrc/model/cbi/ptl-relay/collect.lua  $(1)/usr/lib/lua/luci/model/cbi/ptl-relay/collect.lua
	$(INSTALL_DATA) ./luasrc/model/cbi/ptl-relay/forward.lua  $(1)/usr/lib/lua/luci/model/cbi/ptl-relay/forward.lua

	$(INSTALL_DATA) ./luasrc/model/cbi/ptl-relay/collect-details.lua  $(1)/usr/lib/lua/luci/model/cbi/ptl-relay/collect-details.lua
	$(INSTALL_DATA) ./luasrc/model/cbi/ptl-relay/forward-details.lua  $(1)/usr/lib/lua/luci/model/cbi/ptl-relay/forward-details.lua


	$(INSTALL_DATA) ./luasrc/view/ptl-relay/cbi_add_device.htm $(1)/usr/lib/lua/luci/view/ptl-relay/cbi_add_device.htm
	$(INSTALL_DATA) ./luasrc/view/ptl-relay/cbi_show_tcp_udp_forward.htm $(1)/usr/lib/lua/luci/view/ptl-relay/cbi_show_tcp_udp_forward.htm
	
	$(INSTALL_DATA) ./luasrc/tools/ptl-relay.lua $(1)/usr/lib/lua/luci/tools/ptl-relay.lua

	$(PO2LMO) ./po/ptl-relay.zh-cn.po $(1)/usr/lib/lua/luci/i18n/ptl-relay.zh-cn.lmo

endef

define Package/$(PKG_NAME)/postinst
#!/bin/sh
[ -x /etc/uci-defaults/$(PKG_NAME) ] && /etc/uci-defaults/$(PKG_NAME) || exit 0
endef

define Package/$(PKG_NAME)/postrm
#!/bin/sh
which uci > /dev/null || exit 0
uci -q get ucitrack.@ptl-relay[0] > /dev/null && {
  uci delete ucitrack.@ptl-relay[0]
  uci commit
}
exit 0
endef


$(eval $(call BuildPackage,$(PKG_NAME)))
