# 
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-ser2net
PKG_VERSION=1.0
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

ifeq ($(ARCH),mipsel)
	PO2LMO:=$(STAGING_DIR_HOST)/bin/po2lmo
	ifeq ($(PO2LMO), $(wildcard $(PO2LMO)))
	else
		PO2LMO:=$(STAGING_DIR_HOSTPKG)/bin/po2lmo
	endif
else
	PO2LMO:=$(STAGING_DIR_HOSTPKG)/bin/po2lmo
endif
PWD:=$(shell pwd)

include $(INCLUDE_DIR)/package.mk

define Package/luci-app-ser2net
SECTION:=luci
CATEGORY:=LuCI
SUBMENU:=3. Applications
TITLE:= LuCI support for ser2net
PKGARCH:=all
DEPENDS:=+socat#依赖
endef

define Package/luci-app-ser2net/description
 This package contains LuCI configuration pages for ser2net.
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/luci-app-ser2net/install


	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/root/etc/uci-defaults/uci-defaults-ser2net $(1)/etc/uci-defaults/luci-ser2net

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/i18n
	$(INSTALL_CONF) ./files/root/etc/config/serial_socat $(1)/etc/config/serial_socat
#	$(INSTALL_BIN) ./files/root/etc/init.d/ser2net_client $(1)/etc/init.d/ser2net_client
	$(INSTALL_BIN) ./files/root/etc/init.d/socat_ser2net $(1)/etc/init.d/socat_ser2net

	$(INSTALL_DATA) ./files/root/usr/lib/lua/luci/model/cbi/serial_socat.lua $(1)/usr/lib/lua/luci/model/cbi/serial_socat.lua
	$(INSTALL_DATA) ./files/root/usr/lib/lua/luci/controller/serial_socat.lua $(1)/usr/lib/lua/luci/controller/serial_socat.lua

	$(PO2LMO) ./po/ser2net.zh-cn.po $(1)/usr/lib/lua/luci/i18n/ser2net.zh-cn.lmo
endef


#页面上配置的“保存&应用”功能的实现：
#最终调用到/etc/config/ucitrack的配置文件。
#问题一、那么如何添加到ucitrack配置文件中的呢？ 答：是通过uci-defaults手动添加的。
#Package/$(PKG_NAME)/postinst 软件包安装后处理方法，使用脚本语言。
#Package/$(PKG_NAME)/prerm 软件包删除前处理方法，使用脚本语言。
#Package/$(PKG_NAME)/postrm 软件包删除后处理方法，使用脚本语言。


define Package/$(PKG_NAME)/postinst
#!/bin/sh
[ -x /etc/uci-defaults/luci-ser2net ] && /etc/uci-defaults/luci-ser2net || exit 0
endef

define Package/$(PKG_NAME)/postrm
#!/bin/sh
which uci > /dev/null || exit 0
uci -q get ucitrack.@serial_socat[0] > /dev/null && {
  uci delete ucitrack.@serial_socat[0]
  uci commit
}
exit 0
endef





$(eval $(call BuildPackage,$(PKG_NAME)))
