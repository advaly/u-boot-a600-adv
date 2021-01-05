/*
 * Board functions for Atmark Techno Armadillo-640 WLAN/RTC Option Module
 *
 * Copyright (C) 2018 Atmark Techno, Inc.
 *
 * Kojiro YAMADA <kojiro.yamada@atmark-techno.com>
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
#include <environment.h>
#include <i2c.h>
#include <fuse.h>
#include "board_i2c.h"
#include "board_kotai.h"

#define GPIO_I2C_PAD_CTRL    (PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_LOW | \
	PAD_CTL_DSE_120ohm)

struct i2c_pads_info i2c_gpio_pad_a640 = {
	.scl = {
		.gpio_mode = MX6_PAD_LCD_DATA18__GPIO3_IO23 | MUX_PAD_CTRL(GPIO_I2C_PAD_CTRL),
		.gp = IMX_GPIO_NR(3, 23),
	},
	.sda = {
		.gpio_mode = MX6_PAD_LCD_DATA19__GPIO3_IO24 | MUX_PAD_CTRL(GPIO_I2C_PAD_CTRL),
		.gp = IMX_GPIO_NR(3, 24),
	},
};

struct i2c_pads_info i2c_gpio_pad_a610 = {
	.scl = {
		.gpio_mode = MX6_PAD_CSI_HSYNC__GPIO4_IO20 | MUX_PAD_CTRL(GPIO_I2C_PAD_CTRL),
		.gp = IMX_GPIO_NR(4, 20),
	},
	.sda = {
		.gpio_mode = MX6_PAD_CSI_VSYNC__GPIO4_IO19 | MUX_PAD_CTRL(GPIO_I2C_PAD_CTRL),
		.gp = IMX_GPIO_NR(4, 19),
	},
};

#define NR3225SA_SLAVE_ADDR		0x32
#define NR3225SA_FLAG			0x0C
#define NR3225SA_CTRL			0x0D

#define NR3225SA_FLAG_VDHF                      BIT(5)
#define NR3225SA_FLAG_VDLF                      BIT(4)
#define NR3225SA_FLAG_AF                        BIT(1)
#define NR3225SA_FLAG_TF                        BIT(2)
#define NR3225SA_FLAG_UTF                       BIT(0)

#define NR3225SA_CTRL_UTIE			BIT(0)
#define NR3225SA_CTRL_AIE			BIT(1)
#define NR3225SA_CTRL_TIE			BIT(2)
#define NR3225SA_CTRL_TE			BIT(3)
#define NR3225SA_CTRL_FIE			BIT(4)

#define to_scl_gp(x) (x->scl.gp)
#define to_sda_gp(x) (x->sda.gp)

int soft_i2c_gpio_sda;
int soft_i2c_gpio_scl;

/* Armadillo-640 re-enter power-off state if ONOFF line is kept low
 * voltage for at least 5 seconds. If ONOFF line(CON9_11) is connected
 * to rtc-NR3225SA's INT line, ONOFF line is kept low voltage while
 * the alarm is ringing.  So we stop the ringing alarm. */
static int wlan_rtc_board_init_onoff(void)
{
	int ret;
	uint8_t flag, new_flag, ctrl;

	ret = i2c_probe(NR3225SA_SLAVE_ADDR);
	if (ret)
		return ret;

	flag = i2c_reg_read(NR3225SA_SLAVE_ADDR, NR3225SA_FLAG);
	ctrl = i2c_reg_read(NR3225SA_SLAVE_ADDR, NR3225SA_CTRL);

	new_flag = 0xff;
	if (flag & NR3225SA_FLAG_AF) {
		new_flag &= ~NR3225SA_FLAG_AF;
		ctrl &= ~NR3225SA_CTRL_AIE;
	}
	if (flag & NR3225SA_FLAG_TF) {
		new_flag &= ~NR3225SA_FLAG_TF;
		ctrl &= ~NR3225SA_CTRL_TE;
		ctrl &= ~NR3225SA_CTRL_TIE;
	}
	ctrl &= ~NR3225SA_CTRL_FIE;

	i2c_reg_write(NR3225SA_SLAVE_ADDR, NR3225SA_CTRL, ctrl);
	i2c_reg_write(NR3225SA_SLAVE_ADDR, NR3225SA_FLAG, new_flag);

	return 0;
}

static int wlan_rtc_board_i2c_late_init(void)
{
	struct i2c_pads_info *pads;
	uint32_t val;
	int ret;

	ret = fuse_read(EFUSE_KOTAI_BANK, EFUSE_KOTAI_WORD, &val);
	if (ret) {
		printf("Error: Can not read kotai-code\n");
		return ret;
	}
	switch (val) {
		case EFUSE_KOTAI_A640:
		case EFUSE_KOTAI_A640ES:
			pads = &i2c_gpio_pad_a640;
			break;
		case EFUSE_KOTAI_A610:
		case EFUSE_KOTAI_A610ES:
			pads = &i2c_gpio_pad_a610;
			break;
		default:
			printf("Error: unknown board\n");
			return -ENODEV;
	}

	soft_i2c_gpio_sda = to_sda_gp(pads);
	soft_i2c_gpio_scl = to_scl_gp(pads);
	gpio_request(soft_i2c_gpio_scl, "soft_i2c");
	gpio_request(soft_i2c_gpio_sda, "soft_i2c");
	i2c_force_idle_bus(pads);

	return 0;
}

void wlan_rtc_board_late_init(void)
{
	int ret;
	int i;

	if (env_get_yesno(STOP_NR3225SA_ALARM_ENV_NAME) != 1)
		return;

	if (wlan_rtc_board_i2c_late_init())
		return;

	for (i = 0; i < CONFIG_SYS_NUM_I2C_BUSES; i++) {
		if (!strcmp(I2C_ADAP_NR(i)->name, "soft00")) {
			ret = i2c_set_bus_num(i);
			if (ret)
				break;

		        ret = wlan_rtc_board_init_onoff();
			if (ret)
				break;

			return;
		}
	}
}
