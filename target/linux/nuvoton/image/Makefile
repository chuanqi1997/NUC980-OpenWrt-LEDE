#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

UBIFS_OPTS := -F -m 2048 -e 124KiB -c 848  -U
UBI_OPTS := -m 2048 -p 128KiB -s 2048

#$(PROFILE)_UBI_OPTS :=  -m 2048 -e 124KiB -c 848
#$(PROFILE)_UBIFS_OPTS := -m 2048 -p 128KiB -s 2048

VMLINUX:=$(IMG_PREFIX)-vmlinux
UIMAGE:=$(IMG_PREFIX)-uImage

DEVICE_VARS += DTS MKUBIFS_OPTS

loadaddr-y := 0x00008000
loadaddr-$(CONFIG_TARGET_nuvoton_nuc980) := 0x00008000


KERNEL_LOADADDR := $(loadaddr-y)

#KERNEL_DTB = kernel-bin | patch-dtb | lzma

#define Device/Default
#  PROFILES = Default $$(DTS)
#  KERNEL_DEPENDS = $$(wildcard ../dts/$$(DTS).dts)
#  KERNEL := $(KERNEL_DTB) | uImage lzma
#endef

define Image/Prepare
	###Image/Prepare###
	cp $(LINUX_DIR)/arch/arm/boot/Image $(KDIR)/Image
	cp $(LINUX_DIR)/arch/arm/boot/zImage $(KDIR)/zImage
endef

#cat $(KDIR)/zImage $(DTS_DIR)/$(2).dtb > $(KDIR)/zImage-$(1)
#$(if $(wildcard $(DTS_DIR)/$(2).dtb),$(shell cat $(KDIR)/zImage $(DTS_DIR)/$(2).dtb > $(KDIR)/zImage-$(1)))
#$(if $(wildcard $(KDIR)/$(2).dtb),$(shell cat $(KDIR)/zImage $(KDIR)/$(2).dtb > $(KDIR)/zImage-$(1)))
#$(if $(wildcard ./ubinize.cfg),$(call Image/Build,ubi))
# $(1) source dts file
# $(2) target dtb file
# $(3) extra CPP flags
# $(4) extra DTC flags
#define Image/BuildDTB
#$(if $(wildcard $(KDIR)/$(2).dtb),$(shell cat $(KDIR)/zImage $(KDIR)/$(2).dtb > $(KDIR)/zImage-$(1)))
define MkuImageDtb
	###MkuImageDtb###
	$(call Image/BuildDTB,../dts/$(2).dts,$(KDIR)/$(2).dtb)
	cat $(KDIR)/zImage $(KDIR)/$(2).dtb > $(KDIR)/zImage-$(1)
	mkimage -A arm -T kernel -C none -a $(KERNEL_LOADADDR) -e $(KERNEL_LOADADDR) \
		-n "OpenWrt ARM $(LINUX_VERSION)" \
		-d $(KDIR)/zImage-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage
endef

#dd if=$(KDIR)/nuc980.uImage.align.128k; dd if=$(2) > $(KDIR)/$(output_name)
#cat $(1) $(2) > $(KDIR)/$(output_name)
#cat $(KDIR)/nuc980.uImage.align.128k $(2) > $(KDIR)/$(output_name)
#dd if=$(1) of=$(KDIR)/nuc980.uImage.align.128k bs=128k conv=sync
#$(call prepare_generic_squashfs,$(KDIR)/$(output_name))


ifeq ($(SUBTARGET),nuc980)
include nuc980.mk
endif

# NUC980
#Image/Build/Kernel/NUC980=$(call MkuImageDtb,nuc980-iot-gateway,nuc980-iot-gateway)

#$(call Image/Build/Kernel/NUC980)
#	mkimage -A arm -T kernel -C none -a $(KERNEL_LOADADDR) -e $(KERNEL_LOADADDR) \
#		-d $(KDIR)/Image $(BIN_DIR)/$(IMG_PREFIX)-uImage
#	if [ $(CONFIG_FLEXIBITY_ROOT) ]; then \
#		$(INSTALL_BIN) $(BIN_DIR)/$(IMG_PREFIX)-uImage $(TARGET_DIR)/uImage ; \
#	fi
define Image/BuildKernel
	###Image/BuildKernel###
	$(call MkuImageDtb,nuc980-iot-gateway,nuc980-iot-gateway)
endef


define Image/Build/ubi
#"Image/Build/ubi" should not be run#
endef

define Image/Build/SysupgradeCombined

	mkdir -p "$(KDIR_TMP)/sysupgrade-$(4)/"
	echo "BOARD=$(4)" > "$(KDIR_TMP)/sysupgrade-$(4)/CONTROL"

	$(CP) "$(1)" "$(KDIR_TMP)/sysupgrade-$(4)/kernel"
	$(CP) "$(2)" "$(KDIR_TMP)/sysupgrade-$(4)/root"

	(cd "$(KDIR_TMP)"; $(TAR) cvf \
		"$(BIN_DIR)/$(IMG_PREFIX)-$(3)-sysupgrade.tar" sysupgrade-$(4) \
			$(if $(SOURCE_DATE_EPOCH),--mtime="@$(SOURCE_DATE_EPOCH)") \
	)
endef


#$(eval output_name=$(IMG_PREFIX)-sysupgrade.bin)
#( \
#dd if=$(1) bs=128k conv=sync; \
#dd if=$(2) bs=128k conv=sync; \
#) > $(KDIR)/$(output_name)
#$(call prepare_generic_squashfs,$(KDIR)/$(output_name))
#$(CP) $(KDIR)/$(output_name) $(BIN_DIR)/$(output_name)
define MkImageSysupgrade
	####MkImageSysupgrade-$(3)####
	$(call Image/Build/SysupgradeCombined,$(1),$(2),$(3),NUC980-IOT-GateWay)
endef

define Image/Build/ubifs
	###Image/Build/ubifs###
	$(if $(wildcard ./ubinize.cfg),$(call Image/mkfs/ubifs/generate,))
	$(if $(wildcard ./ubinize-overlay.cfg),$(call Image/mkfs/ubifs/generate,-overlay))
	cp $(KDIR)/root.ubi $(BIN_DIR)/$(IMG_PREFIX)-root-ubi.img
	cp $(KDIR)/root-overlay.ubi $(BIN_DIR)/$(IMG_PREFIX)-root-overlay-ubi.img

	$(call MkImageSysupgrade,$(BIN_DIR)/$(IMG_PREFIX)-nuc980-iot-gateway-uImage,$(KDIR)/root.ubifs,ubifs)
	$(call MkImageSysupgrade,$(BIN_DIR)/$(IMG_PREFIX)-nuc980-iot-gateway-uImage,$(KDIR)/root.squashfs,squashfs)
endef

define Image/Build
	###Image/Build###
	$(call Image/Build/$(1))
	cp $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-root.$(1)
endef

#define BuildFirmware/nuc980/ubifs
#	$(call MkImageLzmaDtb,$(2),$(3),$(4))
#	dd if=$(KDIR)/vmlinux-$(2).uImage of=$(KDIR)/vmlinux-$(2).uImage.align.128k bs=128k conv=sync
#endef

$(eval $(call BuildImage))
