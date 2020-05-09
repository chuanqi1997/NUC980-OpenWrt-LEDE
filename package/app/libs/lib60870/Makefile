include $(TOPDIR)/rules.mk

PKG_NAME:=lib60870
PKG_VERSION:=2.2.1
PKG_RELEASE:=1
#lib60870-2.2.1.tar
#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
#PKG_SOURCE_URL:=https://github.com/mz-automation/lib60870/archive/v$(PKG_VERSION)

#PKG_HASH:=146f2f2e87fcc8c21a990f4519c8a16e5518538f8e354b15663fc6a288d8030e

PKG_LICENSE:=LGPL-2.1
PKG_LICENSE_FILES:=LICENSE
#CMAKE_INSTALL:=0
#PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/lib60870
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=lib60870
  MAINTAINER:= mz-automation 
endef

define Package/lib60870/description
 lib60870 library for IEC 60870-5-101/104 based protocols in C
endef

#CMAKE_OPTIONS += \
#       -DENABLE_LIB_ONLY=ON
#TARGET_CFLAGS += $(FPIC)

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/lib60870-C/* $(PKG_BUILD_DIR)
#$(CP) $(PKG_BUILD_DIR)/lib60870-C/* $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
	$(call Build/Compile/Default,dynlib)
endef

define Build/InstallDev

endef

define Package/lib60870/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/build/lib60870.so $(1)/usr/lib
endef

$(eval $(call BuildPackage,lib60870))
