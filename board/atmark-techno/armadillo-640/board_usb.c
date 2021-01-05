/*
 * Board functions for Atmark Techno Armadillo-640
 *
 * Copyright (C) 2017 Yasushi SHOJI <yashi@atmark-techno.com>
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
#include <usb.h>

/*
 * We only support HOST device for now
 */
int board_usb_phy_mode(int port)
{
	return USB_INIT_HOST;
}

static unsigned int to_gpio(int port) {
	unsigned int gpio;
	switch (port) {
	case 0:
		gpio = IMX_GPIO_NR(1, 19);
		break;
	case 1:
		gpio = IMX_GPIO_NR(4, 17);
		break;
	default:
		panic("Unknown USB Port '%d' is given", port);
		break;
	}

	return gpio;
}

/*
 * Initialize I/O MUX and VBus Power Switch via gpio
 */
int board_ehci_hcd_init(int port)
{
	int value;
	int ret = -ENODEV;

	switch (port) {
	case 0:
		imx_iomux_v3_setup_pad(MX6_PAD_UART1_RTS_B__GPIO1_IO19 | MUX_PAD_CTRL(PAD_CTL_DSE_240ohm));
		value = gpio_get_value(to_gpio(port));
		ret = gpio_direction_output(to_gpio(port), value);

		break;
	case 1:
		imx_iomux_v3_setup_pad(MX6_PAD_CSI_MCLK__GPIO4_IO17 | MUX_PAD_CTRL(PAD_CTL_DSE_240ohm));
		value = gpio_get_value(to_gpio(port));
		ret = gpio_direction_output(to_gpio(port), value);

		break;
	default:
		break;
	}

	if (ret < 0)
		ret = -ENODEV;

	return ret;
}

/*
 * Power on or off the given port
 */
int board_ehci_power(int port, int on)
{
	int ret = -ENODEV;

	debug("port %d %s\n", port, on ? "on" : "off");
	switch (port) {
	case 0:
		ret = gpio_set_value(to_gpio(port), !!on);
		break;
	case 1:
		ret = gpio_set_value(to_gpio(port), !!on);
		break;
	default:
		break;
	}

	if (ret < 0)
		ret = -ENODEV;

	return ret;
}

static void setup_usb_iomux(void)
{
	board_ehci_hcd_init(0);
	board_ehci_hcd_init(1);
}

int setup_usb(void)
{
	setup_usb_iomux();

	return 0;
}
