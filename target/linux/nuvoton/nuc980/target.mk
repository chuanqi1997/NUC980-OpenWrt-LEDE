#
# Copyright (C) 2009 OpenWrt.org
#

SUBTARGET:=nuc980
BOARDNAME:=nuc980 based boards
FEATURES+=usb
CPU_TYPE:=arm926ej-s

DEFAULT_PACKAGES +=
KERNELNAME:=Image dtbs uImage

define Target/Description
	nuc980 target
endef
