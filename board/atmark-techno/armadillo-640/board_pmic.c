/*
 * Board functions for Atmark Techno Armadillo-640 WLAN/RTC Option Module
 *
 * Copyright (C) 2018 Atmark Techno, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/iomux.h>
#include <asm/gpio.h>
#include <power/pmic.h>
#include <power/pfuze3000_pmic.h>
#include <environment.h>
#include <i2c.h>
#include "board_i2c.h"

#define I2C_PAD_CTRL (PAD_CTL_PKE | PAD_CTL_PUE |		\
		      PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |	\
		      PAD_CTL_DSE_40ohm | PAD_CTL_HYS |		\
		      PAD_CTL_ODE)
#define PC MUX_PAD_CTRL(I2C_PAD_CTRL)

/* I2C1 for PMIC */
static struct i2c_pads_info i2c_pad_info1 = {
	.scl = {
		.i2c_mode =  MX6_PAD_UART4_TX_DATA__I2C1_SCL | PC,
		.gpio_mode = MX6_PAD_UART4_TX_DATA__GPIO1_IO28 | PC,
		.gp = IMX_GPIO_NR(1, 28),
	},
	.sda = {
		.i2c_mode = MX6_PAD_UART4_RX_DATA__I2C1_SDA | PC,
		.gpio_mode = MX6_PAD_UART4_RX_DATA__GPIO1_IO29 | PC,
		.gp = IMX_GPIO_NR(1, 29),
	},
};

#define I2C_PMIC       0
int pmic_late_init(void)
{
	struct pmic *dev;
	int ret;
	u32 dev_id, rev_id;

	i2c_force_idle_bus(&i2c_pad_info1);
	setup_i2c(I2C_PMIC, CONFIG_SYS_I2C_SPEED, 0x7f, &i2c_pad_info1);

	ret = power_pfuze3000_init(I2C_PMIC);
	if (ret)
		return ret;

	dev = pmic_get("PFUZE3000");
	ret = pmic_probe(dev);
	if (ret)
		return ret;

	pmic_reg_read(dev, PFUZE3000_DEVICEID, &dev_id);
	pmic_reg_read(dev, PFUZE3000_REVID, &rev_id);
	printf("PMIC: PFUZE3000 DEV_ID=0x%x REV_ID=0x%x\n", dev_id, rev_id);

	/* Low Power Mode during standby mode */
	pmic_reg_write(dev, PFUZE3000_LDOGCTL, !env_get_yesno(ENABLE_PF3000_LPM_ENV_NAME));

	return 0;
}
