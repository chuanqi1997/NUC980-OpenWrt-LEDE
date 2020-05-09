#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=nuvoton
BOARDNAME:=Nuvoton ARM9
SUBTARGETS:=nuc980
FEATURES:=squashfs nand targz usb usbgadget ubifs gpio
MAINTAINER:=legend 466005858@qq.com

KERNEL_PATCHVER:=4.4

define Target/Description
	Nuvoton ARM9 NUC980 Target.
endef

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=uImage dtbs

DEFAULT_PACKAGES += \
	kmod-leds-gpio wpad-mini kmod-gpio-button-hotplug

$(eval $(call BuildTarget))
