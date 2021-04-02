/*
 * Board functions for Atmark Techno Armadillo-640
 *
 * Copyright (C) 2017 Yasushi SHOJI <yashi@atmark-techno.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#include <stdbool.h>
#include <linux/types.h>
#include <asm/types.h>
#include <asm/u-boot.h>
#include <asm/global_data.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/gpio.h>
#include <fuse.h>
#include "fec_mxc.h"
#include "board_usb.h"
#include "sdhc_mxc.h"
#include "board_wlan_rtc.h"
#include "board_pmic.h"
#include "board_kotai.h"

DECLARE_GLOBAL_DATA_PTR;

#define RED_LED IMX_GPIO_NR(1, 5)
#define GREEN_LED IMX_GPIO_NR(1, 8)
#define YELLOW_LED IMX_GPIO_NR(1, 18)

#define MAINTENANCE_MODE_GPIO_CON9_1 IMX_GPIO_NR(1, 22)
#define MAINTENANCE_MODE_GPIO_SW1 IMX_GPIO_NR(1, 10)
static iomux_v3_cfg_t maintenance_mode_gpio_pads[] = {
	MX6_PAD_UART2_CTS_B__GPIO1_IO22 | MUX_PAD_CTRL(PAD_CTL_PUS_22K_UP | PAD_CTL_PUE),
	MX6_PAD_JTAG_MOD__GPIO1_IO10 | MUX_PAD_CTRL(PAD_CTL_HYS | PAD_CTL_PUS_100K_DOWN | PAD_CTL_DSE_DISABLE),
};

static iomux_v3_cfg_t status_led_gpio_pads[] = {
	MX6_PAD_GPIO1_IO05__GPIO1_IO05 | MUX_PAD_CTRL(PAD_CTL_DSE_240ohm),  /* LED3:Red */
	MX6_PAD_GPIO1_IO08__GPIO1_IO08 | MUX_PAD_CTRL(PAD_CTL_DSE_240ohm),  /* LED4:Green */
	MX6_PAD_UART1_CTS_B__GPIO1_IO18 | MUX_PAD_CTRL(PAD_CTL_DSE_240ohm), /* LED5:Yellow */
};

static void setup_maintenance_sw(void)
{
	imx_iomux_v3_setup_multiple_pads(maintenance_mode_gpio_pads,
					 ARRAY_SIZE(maintenance_mode_gpio_pads));
	gpio_direction_input(MAINTENANCE_MODE_GPIO_CON9_1);
	gpio_direction_input(MAINTENANCE_MODE_GPIO_SW1);
}

static void set_fdt_file(void)
{
	uint32_t val;
	int ret;

	if (env_get("fdt_file"))
		return;

	ret = fuse_read(EFUSE_KOTAI_BANK, EFUSE_KOTAI_WORD, &val);
	if (ret) {
		printf("Error: Can not read kotai-code\n");
		return;
	}
	switch (val) {
		case EFUSE_KOTAI_A640:
		case EFUSE_KOTAI_A640ES:
			env_set("fdt_file", "a640.dtb");
			break;
		case EFUSE_KOTAI_A610:
		case EFUSE_KOTAI_A610ES:
			env_set("fdt_file", "a610.dtb");
			break;
		default:
			printf("Error: unknown board. "
			       "set env `fdt_file' for boot.\n");
			break;
	}
}

static void setup_slot(void)
{
	if (gpio_get_value(MAINTENANCE_MODE_GPIO_SW1)) {
		/* Boot as recovery mode */
		env_set("slot", "1");
	} else {
		if (env_get("slot") == NULL) {
			/* Boot from slot 2 as default */
			env_set("slot", "2");
		}
	}
}

void coloured_LED_init(void)
{
	imx_iomux_v3_setup_multiple_pads(status_led_gpio_pads,
			       ARRAY_SIZE(status_led_gpio_pads));
	gpio_direction_output(GREEN_LED, 1);
}

int board_init(void)
{
	setup_maintenance_sw();
	setup_fec();
	setup_usb();

	return 0;
}

int board_late_init(void)
{
	wlan_rtc_board_late_init();
	pmic_late_init();
	set_fdt_file();
	setup_slot();

	return 0;
}

int dram_init(void)
{
	gd->ram_size = imx_ddr_size();
	return 0;
}

#ifdef CONFIG_BOOTSTAGE
extern unsigned long __weak timer_read_counter(void);
ulong timer_get_boot_us(void)
{
	return timer_read_counter();
}
#endif

#if defined(CONFIG_SUPPORT_MAINTENANCE_MODE)
int is_maintenance_mode(void)
{
	return !gpio_get_value(MAINTENANCE_MODE_GPIO_CON9_1);
}
#endif
