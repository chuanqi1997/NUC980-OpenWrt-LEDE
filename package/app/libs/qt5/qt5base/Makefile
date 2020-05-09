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

include $(sort $(wildcard package/app/libs/qt5/*/*.mk))

# Variable for other Qt applications to use
QT5_QMAKE = $(HOST_DIR)/bin/qmake -spec linux-openwrt-g++


QT5_SITE = https://download.qt.io/archive/qt/5.12/$(5.12.7)/submodules

PKG_NAME:=qt5
PKG_VERSION:=$(QT5_VERSION)
PKG_RELEASE:=1
PKG_MD5SUM:=4c3a9759ffd50b004f9b40c0948f44d0

QT5BASE_VERSION = $(QT5_VERSION)
QT5BASE_SITE = $(QT5_SITE)
QT5BASE_SOURCE = qtbase-$(QT5_SOURCE_TARBALL_PREFIX)-$(QT5BASE_VERSION).tar.xz

PKG_SOURCE:=${QT5BASE_SOURCE}
PKG_SOURCE_URL:=${QT5BASE_SITE}

PKG_BUILD_DIR=$(BUILD_DIR)/qtbase-$(QT5_SOURCE_TARBALL_PREFIX)-$(QT5BASE_VERSION)
PKG_BUILD_PARALLEL:=1
PKG_BUILD_DEPENDS:=+libstdcpp

include $(INCLUDE_DIR)/package.mk
#-include $(if $(DUMP),,./files/qmake.mk)

define Package/qt5/Default
  SECTION:=libraries
  CATEGORY:=Libraries
  TITLE:=qt5
  URL:=http://qt-project.org
  DEPENDS:=+librt +zlib +libstdcpp +libpthread
endef

define Package/qt5-core
  $(call Package/qt5/Default)
  TITLE+=qt5-core
endef

define Package/qt5-concurrent
  $(call Package/qt5/Default)
  TITLE+=concurrent
  DEPENDS+=+qt5-core
endef

define Package/qt5-network
  $(call Package/qt5/Default)
  TITLE+=network
  DEPENDS+=+qt5-core
endef

define Package/qt5-xml
  $(call Package/qt5/Default)
  TITLE+=network
  DEPENDS+=+qt5-core
endef

define Package/qt5-sql
  $(call Package/qt5/Default)
  TITLE+=network
  DEPENDS+=+qt5-core
endef

define Build/Configure
	mkdir -p $(PKG_BUILD_DIR)/mkspecs/linux-openwrt-g++
	$(CP) ./$(FILEPFX)qmake.conf $(PKG_BUILD_DIR)/mkspecs/linux-openwrt-g++/qmake.conf
	$(CP) ./$(FILEPFX)qplatformdefs.h $(PKG_BUILD_DIR)/mkspecs/linux-openwrt-g++/qplatformdefs.h
	echo ####TARGET_CROSS:"$(TARGET_CROSS)gcc"####
	( cd $(PKG_BUILD_DIR) ; \
		TARGET_CC="$(TARGET_CROSS)gcc" \
		TARGET_CXX="$(TARGET_CROSS)g++" \
		TARGET_AR="$(TARGET_CROSS)ar cqs" \
		TARGET_OBJCOPY="$(TARGET_CROSS)objcopy" \
		TARGET_RANLIB="$(TARGET_CROSS)ranlib" \
		TARGET_CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
		TARGET_CXXFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
		TARGET_LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS) -lpthread -lrt" \
		TARGET_INCDIRS="$(TARGET_INCDIRS)" \
		TARGET_LIBDIRS="$(TARGET_LIBDIRS) $(STAGING_DIR)/usr/lib/" \
		CFLAGS= \
		CXXFLAGS= \
		LDFLAGS= \
		./configure \
			-xplatform linux-openwrt-g++ \
			-prefix $(CONFIGURE_PREFIX) \
			-bindir $(CONFIGURE_PREFIX)/bin \
			-libdir $(CONFIGURE_PREFIX)/lib \
			-datadir $(CONFIGURE_PREFIX)/share/Qt \
			-plugindir $(CONFIGURE_PREFIX)/lib/Qt/plugins \
			-opensource \
			-confirm-license \
			-no-opengl \
			-make libs \
			-v \
	)
endef

define Build/Compile
	TARGET_CC="$(TARGET_CROSS)gcc" \
	TARGET_CXX="$(TARGET_CROSS)g++" \
	TARGET_AR="$(TARGET_CROSS)ar cqs" \
	TARGET_OBJCOPY="$(TARGET_CROSS)objcopy" \
	TARGET_RANLIB="$(TARGET_CROSS)ranlib" \
	TARGET_CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
	TARGET_CXXFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
	TARGET_LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS) -ldl -lpthread -lrt" \
	TARGET_INCDIRS="$(TARGET_INCDIRS)" \
	TARGET_LIBDIRS="$(TARGET_LIBDIRS) $(STAGING_DIR)/usr/lib/" \
	STAGING_DIR="$(STAGING_DIR)" \
	STAGING_DIR_HOST="$(STAGING_DIR)/../host" \
	PKG_CONFIG_SYSROOT="$(STAGING_DIR)" \
	$(MAKE) -C $(PKG_BUILD_DIR)
	INSTALL_ROOT=$(PKG_INSTALL_DIR) \
	$(MAKE) -C $(PKG_BUILD_DIR) install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(2)/bin 
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_DIR) $(1)/usr/mkspecs 
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(INSTALL_DIR) $(1)/usr/lib/cmake

	$(INSTALL_DIR) $(STAGING_DIR)/mk/
	$(INSTALL_DATA) ./qmake.mk $(STAGING_DIR)/mk/

	$(CP) \
		$(PKG_INSTALL_DIR)/usr/bin/* \
		$(2)/bin/
		
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/bin/* \
		$(2)/bin/

	$(CP) \
		$(PKG_INSTALL_DIR)/usr/mkspecs/* \
		$(1)/usr/mkspecs/

	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/* \
		$(1)/usr/lib/pkgconfig/

	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/* \
		$(1)/usr/include/
		
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/*.so* \
		$(1)/usr/lib/

	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/*.prl* \
		$(1)/usr/lib/
	
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/*.la* \
		$(1)/usr/lib/

	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/cmake/* \
		$(1)/usr/lib/cmake/
endef

define Package/qt5-core/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Core.so* \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Core.prl \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Core.la \
		$(1)/usr/lib/
endef

define Package/qt5-concurrent/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Concurrent.so* \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Concurrent.prl \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Concurrent.la \
		$(1)/usr/lib/
endef

define Package/qt5-network/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Network.so* \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Network.prl \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Network.la \
		$(1)/usr/lib/
endef

define Package/qt5-xml/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Xml.so* \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Xml.prl \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Xml.la \
		$(1)/usr/lib/
endef

define Package/qt5-sql/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Sql.so* \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Sql.prl \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5Sql.la \
		$(1)/usr/lib/
endef

define Package/qt5-xmlpatterns/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5XmlPatterns.so* \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5XmlPatterns.prl \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libQt5XmlPatterns.la \
		$(1)/usr/lib/
endef

$(eval $(call BuildPackage,qt5-core))
$(eval $(call BuildPackage,qt5-concurrent))
$(eval $(call BuildPackage,qt5-network))
$(eval $(call BuildPackage,qt5-sql))
$(eval $(call BuildPackage,qt5-xml))
