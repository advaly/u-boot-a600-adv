/*
 * Copyright (C) 2017 Yasushi SHOJI <yashi@atmark-techno.com>
 *
 * Configuration settings for Atmark Techno Armadillo-640
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef __ARMADILLO640_CONFIG_H
#define __ARMADILLO640_CONFIG_H

#include "mx6_common.h"

#ifdef CONFIG_SPL
#include "imx6_spl.h"
#endif

/* cancel autoboot for maintenance mode */
#if defined(CONFIG_SUPPORT_MAINTENANCE_MODE) && !defined(__ASSEMBLY__)
int is_maintenance_mode(void);
#endif

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_SDRAM_BASE 0x80000000

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(16 * SZ_1M)

#define CONFIG_ENV_SIZE			SZ_512K
#define CONFIG_ENV_OFFSET		SZ_512K

/* Memory Map */
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_RAM_SIZE)

/* CONFIG_MXC_UART */

#if defined CONFIG_CONSOLE_UART1
#define CONFIG_MXC_UART_BASE		UART1_BASE
#elif defined CONFIG_CONSOLE_UART3
#define CONFIG_MXC_UART_BASE		UART3_BASE
#endif

/* USB Configs */
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET
#define CONFIG_MXC_USB_PORTSC  (PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS   0
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2
#ifdef CONFIG_USB_STORAGE
#define BOOTCOMMAND_USB \
	"setup_bootcmd_usb=setenv bootcmd run setup_usbargs\\\\; usb start\\\\; ext4load usb 0:2 \\\\${loadaddr} /boot/uImage\\\\; ext4load usb 0:2 0x83000000 /boot/\\\\${fdt_file}\\\\; usb stop\\\\; bootm \\\\${loadaddr} - 0x83000000\\\\;\0"\
	"setup_usbargs=setenv bootargs root=/dev/sda2 rootwait rw ${optargs};\0"\
	"usbboot=run setup_bootcmd_usb; boot;\0"
#else
#define BOOTCOMMAND_USB ""
#endif

/* I2C Configs */
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC_I2C1
#define CONFIG_SYS_I2C_SPEED		100000
#define CONFIG_SYS_I2C_SOFT
#define CONFIG_SYS_I2C_SOFT_SPEED	100000
#define CONFIG_SYS_I2C_INIT_BOARD

#define CONFIG_SOFT_I2C_GPIO_SDA	soft_i2c_gpio_sda
#define CONFIG_SOFT_I2C_GPIO_SCL	soft_i2c_gpio_scl
#ifndef __ASSEMBLY__
extern int soft_i2c_gpio_sda;
extern int soft_i2c_gpio_scl;
#endif

/* PMIC */
#define CONFIG_POWER
#define CONFIG_POWER_I2C
#define CONFIG_POWER_PFUZE3000
#define CONFIG_POWER_PFUZE3000_I2C_ADDR	0x08

#ifndef __ASSEMBLY__
void wlan_rtc_i2c_scl(int bit);
void wlan_rtc_i2c_sda(int bit);
int wlan_rtc_i2c_read(void);
#endif

#define I2C_INIT
#define I2C_ACTIVE
#define I2C_TRISTATE

/* MMC Configs */
#define CONFIG_FSL_ESDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR  USDHC1_BASE_ADDR
#define CONFIG_SYS_MMC_ENV_DEV     0

/* Ethernet */
#ifdef CONFIG_FEC_MXC
#define CONFIG_FEC_XCV_TYPE RMII
#define CONFIG_MII
#define CONFIG_FEC_MXC_PHYADDR		0
#define IMX_FEC_BASE			ENET_BASE_ADDR
#endif

#ifdef CONFIG_WLAN_RTC_OPTION_MODULE_INIT_ONOFF
#define STOP_NR3225SA_ALARM_DEFAULT	"yes"
#else
#define STOP_NR3225SA_ALARM_DEFAULT	"no"
#endif
#define STOP_NR3225SA_ALARM_ENV_NAME	"stop_nr3225sa_alarm"

#ifdef CONFIG_PF3000_LOW_POWER_MODE_ENABLE
#define ENABLE_PF3000_LPM_DEFAULT	"yes"
#else
#define ENABLE_PF3000_LPM_DEFAULT	"no"
#endif
#define ENABLE_PF3000_LPM_ENV_NAME	"enable_pf3000_lpm"

#define CONFIG_BOOTCOMMAND \
	"run setup_mmcargs; ext4load mmc 0:${slot} ${loadaddr} /boot/uImage; ext4load mmc 0:${slot} 0x83000000 /boot/${fdt_file}; bootm ${loadaddr} - 0x83000000; run setup_mmcargs_recovery; ext4load mmc 0:1 ${loadaddr} /uImage; ext4load mmc 0:1 0x83000000 /${fdt_file}; bootm ${loadaddr} - 0x83000000;"
#define CONFIG_EXTRA_ENV_SETTINGS \
	"setup_mmcargs=setenv bootargs root=/dev/mmcblk0p${slot} rootwait rw ${optargs};\0"\
	"setup_mmcargs_recovery=setenv bootargs ${optargs};\0"\
	BOOTCOMMAND_USB\
	"tftpboot=tftpboot uImage; tftpboot 0x83000000 ${fdt_file}; bootm ${loadaddr} - 0x83000000;\0"\
	STOP_NR3225SA_ALARM_ENV_NAME "=" STOP_NR3225SA_ALARM_DEFAULT ";\0"\
	ENABLE_PF3000_LPM_ENV_NAME "=" ENABLE_PF3000_LPM_DEFAULT "\0"

#define CONFIG_BOARD_LATE_INIT


#endif
