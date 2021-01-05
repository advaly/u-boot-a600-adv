/*
 * Board functions for Atmark Techno Armadillo-640
 *
 * Copyright (C) 2018 Atmark Techno, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/iomux.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <asm/gpio.h>

void i2c_force_idle_bus(struct i2c_pads_info *pins)
{
	int i;

	gpio_direction_input(pins->sda.gp);
	gpio_direction_input(pins->scl.gp);

	imx_iomux_v3_setup_pad(pins->sda.gpio_mode);
	imx_iomux_v3_setup_pad(pins->scl.gpio_mode);

	/* send start/stop bit */
	udelay(5);
	gpio_direction_output(pins->sda.gp, 0);
	udelay(5);
	gpio_direction_input(pins->sda.gp);
	udelay(5);

	/* Send high and low on the SCL line */
	for (i = 0; i < 54; i++) {
		gpio_direction_output(pins->scl.gp, 0);
		udelay(5);
		gpio_direction_input(pins->scl.gp);
		udelay(5);
	}

	/* send stop bit */
	gpio_direction_output(pins->scl.gp, 0);
	gpio_direction_output(pins->sda.gp, 0);
	udelay(5);
	gpio_direction_input(pins->scl.gp);
	udelay(5);
	gpio_direction_input(pins->sda.gp);
}
