#
# Copyright (C) 2020 legend 466005858@qq.com
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
# 

include $(TOPDIR)/rules.mk


################################################################################
#
# qt5
#
################################################################################

QT5_VERSION_MAJOR = 5.12
QT5_VERSION = $(QT5_VERSION_MAJOR).7
QT5_SOURCE_TARBALL_PREFIX = everywhere-src
QT5_SITE = https://download.qt.io/archive/qt/$(QT5_VERSION_MAJOR)/$(QT5_VERSION)/submodules

QT5_SITE = https://download.qt.io/archive/qt/5.12/$(5.12.7)/submodules

PKG_NAME:=qtserialbus
PKG_VERSION:=$(QT5_VERSION)
PKG_RELEASE:=1
PKG_MD5SUM:=54a85873439b0d9765cd1a662b2a0ed7

QT5BASE_VERSION = $(QT5_VERSION)
QT5BASE_SITE = $(QT5_SITE)
QT5BASE_SOURCE = ${PKG_NAME}-$(QT5_SOURCE_TARBALL_PREFIX)-$(QT5BASE_VERSION).tar.xz

PKG_SOURCE:=${QT5BASE_SOURCE}
PKG_SOURCE_URL:=${QT5BASE_SITE}

PKG_BUILD_DIR=$(BUILD_DIR)/${PKG_NAME}-$(QT5_SOURCE_TARBALL_PREFIX)-$(QT5BASE_VERSION)
PKG_BUILD_PARALLEL:=1
PKG_BUILD_DEPENDS:=+libstdcpp

QT5_BASE_PKG_BUILD_DIR=$(BUILD_DIR)/qtbase-$(QT5_SOURCE_TARBALL_PREFIX)-$(QT5BASE_VERSION)
# Variable for other Qt applications to use
QT5_QMAKE = $(QT5_BASE_PKG_BUILD_DIR)/bin/qmake -spec linux-openwrt-g++

include $(INCLUDE_DIR)/package.mk

define 	Package/${PKG_NAME}
  CATEGORY:=Libraries
  SECTION:=libraries
  TITLE:=${PKG_NAME}
  DEPENDS:=+qt5-core qtserialport
endef

ifeq ($(CONFIG_CCACHE),)
  CMAKE_C_COMPILER:=$(TOOLCHAIN_DIR)/bin/$(TARGET_CC)
  CMAKE_C_COMPILER_ARG1:=
  CMAKE_CXX_COMPILER:=$(TOOLCHAIN_DIR)/bin/$(TARGET_CXX)
  CMAKE_CXX_COMPILER_ARG1:=
else
  CMAKE_C_COMPILER:=$(STAGING_DIR_HOST)/bin/ccache
  CMAKE_C_COMPILER_ARG1:=$(filter-out ccache,$(TARGET_CC))
  CMAKE_CXX_COMPILER:=$(STAGING_DIR_HOST)/bin/ccache
  CMAKE_CXX_COMPILER_ARG1:=$(filter-out ccache,$(TARGET_CXX))
endif

define Build/Configure

	$(INSTALL_DIR) $(PKG_INSTALL_DIR)/usr/include/QtSerialBus
	$(CP) $(PKG_BUILD_DIR)/src/serialbus/*.h $(PKG_INSTALL_DIR)/usr/include/QtSerialBus/
	( cd $(PKG_BUILD_DIR); \
		CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
		CXXFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
		LD_LIBRARY_PATH="$(TARGET_LIBDIRS) $(STAGING_DIR)/usr/lib/" \
	)
	${QT5_QMAKE} $(PKG_BUILD_DIR)/${PKG_NAME}.pro -o $(PKG_BUILD_DIR)
	#echo ####${PKG_NAME}  Build/Configure ####
	#echo ##${QT5_QMAKE} $(PKG_BUILD_DIR)/${PKG_NAME}.pro##

endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/QtSerialBus
	$(INSTALL_DIR) $(1)/usr/lib

	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/QtSerialBus/* \
		$(1)/usr/include/QtSerialBus/

	$(CP) \
		$(PKG_BUILD_DIR)/lib/libQt5SerialBus.so* \
		$(1)/usr/lib/
endef

define Package/qtserialport/install
	$(INSTALL_DIR) $(1)/usr/lib
	
	$(CP) \
		$(PKG_BUILD_DIR)/lib/libQt5SerialBus.so* \
		$(1)/usr/lib/
endef

$(eval $(call BuildPackage,${PKG_NAME}))
