include $(TOPDIR)/rules.mk

PKG_NAME:=libiec61850
PKG_VERSION:=1.4.0
PKG_RELEASE:=1
#v1.4.0.tar.gz
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/mz-automation/libiec61850/archive/v$(PKG_VERSION)
PKG_HASH:=027fed152aca9ba8e31d87e6b765865745584d892644bf041315447f0d1ef9f4

PKG_LICENSE:=LGPL-2.1
PKG_LICENSE_FILES:=LICENSE
#CMAKE_INSTALL:=1
#PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libiec61850
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libiec61850
  MAINTAINER:= mz-automation 
endef

define Package/libiec61850/description
 libiec61850 library for IEC 61850 based protocols in C
endef

#CMAKE_OPTIONS += \
#       -DENABLE_LIB_ONLY=ON
TARGET_CFLAGS += $(FPIC)

define Build/Prepare#－－将源码copy到编译路径。即build_dir/target-XXXXX/下面，进行编译
	#PKG_BUILD_DIR:= $(PKG_BUILD_DIR)/lib60870-C/
	#mkdir -p $(PKG_BUILD_DIR)
	#$(call Build/Prepare/Default)
	#$(CP) $(PKG_BUILD_DIR)/lib60870-C/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/libiec61850-1.4.0/* $(PKG_BUILD_DIR)
endef
define Build/Compile
	$(call Build/Compile/Default,dynlib)
endef

define Build/InstallDev

endef

define Package/libiec61850/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/build/libiec61850.so $(1)/usr/lib
endef

$(eval $(call BuildPackage,libiec61850))
