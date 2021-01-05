#
# Copyright (C) 2017 Yasushi SHOJI <yashi@atmark-techno.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#
obj-y	:= board.o board_wlan_rtc.o board_pmic.o
obj-$(CONFIG_SPL_BUILD) += spl.o
obj-$(CONFIG_BOARD_EARLY_INIT_F) += board_early_init_f.o
obj-$(CONFIG_FEC_MXC) += fec_mxc.o
obj-$(CONFIG_SYS_I2C) += board_i2c.o
obj-$(CONFIG_USB_EHCI_MX6) += board_usb.o
obj-$(CONFIG_FSL_ESDHC) += sdhc_mxc.o
