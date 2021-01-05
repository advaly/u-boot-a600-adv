/*
 * Board functions for Atmark Techno Armadillo-640
 *
 * Copyright (C) 2018 Atmark Techno, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#include <asm/mach-imx/mxc_i2c.h>

#ifdef CONFIG_SYS_I2C
void i2c_force_idle_bus(struct i2c_pads_info *pins);
#else
void i2c_force_idle_bus(struct i2c_pads_info *pins) {}
#endif
