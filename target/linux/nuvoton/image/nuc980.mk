#
# NUC980 Profiles
#

define Device/nuc980-iot
  DEVICE_DTS:= nuc980-iot-gateway
  DTS := nuc980-iot-gateway
  DEVICE_TITLE := nuc980-iot-gw
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
  DEVICE_NAME := nuc980
  DEVICE_PROFILE := nuc980
#  IMAGES := root.ubifs
#  IMAGES += sysupgrade.bin
  PAGESIZE := 2048
  BLOCKSIZE := 128k
  MKUBIFS_OPTS := -m $$(PAGESIZE) -e 124KiB
  DEVICE_DTS_DIR := ../dts
  KERNEL_DEPENDS := $$(wildcard ../dts/$$(DTS).dts)
#  KERNEL += kernel-bin | patch-dtb | none | uImage
#  KERNEL += kernel-bin | patch-dtb | none | uImage
#  KERNEL_NAME := zImage
  FILESYSTEMS += ubifs
endef
TARGET_DEVICES += nuc980-iot



define Device/nuc980-cm100
  DEVICE_DTS:= nuc980-iot-cm100
  DTS := nuc980-iot-cm100
  DEVICE_TITLE := nuc980-cm100
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
  DEVICE_NAME := nuc980
  DEVICE_PROFILE := nuc980
#  IMAGES := root.ubifs
#  IMAGES += sysupgrade.bin
  PAGESIZE := 2048
  BLOCKSIZE := 128k
  MKUBIFS_OPTS := -m $$(PAGESIZE) -e 124KiB
  DEVICE_DTS_DIR := ../dts
  KERNEL_DEPENDS := $$(wildcard ../dts/$$(DTS).dts)
#  KERNEL += kernel-bin | patch-dtb | none | uImage
#  KERNEL += kernel-bin | patch-dtb | none | uImage
#  KERNEL_NAME := zImage
  FILESYSTEMS += ubifs
endef
TARGET_DEVICES += nuc980-cm100

define Device/nuc980-psm100
  DEVICE_DTS:= nuc980-iot-psm100
  DTS := nuc980-iot-psm100
  DEVICE_TITLE := nuc980-psm100
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
  DEVICE_NAME := nuc980
  DEVICE_PROFILE := nuc980
#  IMAGES := root.ubifs
#  IMAGES += sysupgrade.bin
  PAGESIZE := 2048
  BLOCKSIZE := 128k
  MKUBIFS_OPTS := -m $$(PAGESIZE) -e 124KiB
  DEVICE_DTS_DIR := ../dts
  KERNEL_DEPENDS := $$(wildcard ../dts/$$(DTS).dts)
#  KERNEL += kernel-bin | patch-dtb | none | uImage
#  KERNEL += kernel-bin | patch-dtb | none | uImage
#  KERNEL_NAME := zImage
  FILESYSTEMS += ubifs
endef
TARGET_DEVICES += nuc980-psm100

