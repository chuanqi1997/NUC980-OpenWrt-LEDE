# 
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-sub-ghz
PKG_VERSION=1.0
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
#PO2LMO:=$(STAGING_DIR_HOST)/bin/po2lmo
PO2LMO:=$(STAGING_DIR_HOSTPKG)/bin/po2lmo
PWD:=$(shell pwd)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
SECTION:=luci
CATEGORY:=LuCI
SUBMENU:=3. Applications
TITLE:= LuCI support for SUB-Ghz
PKGARCH:=all
DEPENDS:=+socat +lua-rs232#依赖
endef

define Package/$(PKG_NAME)/description
 This package contains LuCI configuration pages for SUB-GHZ.
endef

define Build/Prepare
endef

define Build/Configure
endef

#define Build/Compile
#	($(TARGET_CC)  ./librs232/src/rs232.c ./librs232/src/rs232_posix.c ./librs232/bindings/lua/luars232.c -DLUAROCKS_HACK -std=gnu99 -I ./#librs232/include -I$(STAGING_DIR)/usr/include -L$(STAGING_DIR)/usr/lib -fPIC -shared -o luars232.so)
#endef

define Build/Compile
endef


define Package/$(PKG_NAME)/install

	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/root/etc/uci-defaults/uci-defaults-sub-ghz $(1)/etc/uci-defaults/luci-sub-ghz

#	$(INSTALL_DIR) $(1)/usr/lib/lua
#	$(INSTALL_BIN) ./luars232.so  $(1)/usr/lib/lua/


	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/i18n

	$(INSTALL_CONF) ./files/root/etc/config/sub-ghz $(1)/etc/config/sub-ghz
	$(INSTALL_BIN) ./files/root/etc/init.d/sub-ghz $(1)/etc/init.d/sub-ghz
	$(INSTALL_BIN) ./files/root/usr/sbin/sub-ghz-at-cmd.lua $(1)/usr/sbin/sub-ghz-at-cmd.lua




	$(INSTALL_DATA) ./files/root/usr/lib/lua/luci/model/cbi/sub-ghz.lua $(1)/usr/lib/lua/luci/model/cbi/sub-ghz.lua
	$(INSTALL_DATA) ./files/root/usr/lib/lua/luci/controller/sub-ghz.lua $(1)/usr/lib/lua/luci/controller/sub-ghz.lua

	$(PO2LMO) ./po/sub-ghz.zh-cn.po $(1)/usr/lib/lua/luci/i18n/sub-ghz.zh-cn.lmo
endef

#页面上配置的“保存&应用”功能的实现：
#最终调用到/etc/config/ucitrack的配置文件。
#问题一、那么如何添加到ucitrack配置文件中的呢？ 答：是通过uci-defaults手动添加的。
#Package/$(PKG_NAME)/postinst 软件包安装后处理方法，使用脚本语言。
#Package/$(PKG_NAME)/prerm 软件包删除前处理方法，使用脚本语言。
#Package/$(PKG_NAME)/postrm 软件包删除后处理方法，使用脚本语言。


define Package/$(PKG_NAME)/postinst
#!/bin/sh
[ -x /etc/uci-defaults/luci-sub-ghz ] && /etc/uci-defaults/luci-sub-ghz || exit 0
endef

define Package/$(PKG_NAME)/postrm
#!/bin/sh
which uci > /dev/null || exit 0
uci -q get ucitrack.@sub-ghz[0] > /dev/null && {
  uci delete ucitrack.@sub-ghz[0]
  uci commit
}
exit 0
endef





$(eval $(call BuildPackage,$(PKG_NAME)))
