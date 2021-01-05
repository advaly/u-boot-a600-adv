/*
 * Board functions for Atmark Techno Armadillo-640
 *
 * Copyright (C) 2017 Yasushi SHOJI <yashi@atmark-techno.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#include <asm/arch/mx6-pins.h>
#include <asm/mach-imx/iomux-v3.h>

#define UART_PAD_CTRL (PAD_CTL_PKE         | PAD_CTL_PUE |		\
		       PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |	\
		       PAD_CTL_DSE_40ohm   | PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#if defined CONFIG_CONSOLE_UART1
static iomux_v3_cfg_t const uart1_pads[] = {
	MX6_PAD_UART1_TX_DATA__UART1_DCE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_UART1_RX_DATA__UART1_DCE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
};
#elif defined CONFIG_CONSOLE_UART3
static iomux_v3_cfg_t const uart3_pads[] = {
	MX6_PAD_UART3_TX_DATA__UART3_DCE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_UART3_RX_DATA__UART3_DCE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
};
#endif

static void setup_iomux_uart(void)
{
#if defined CONFIG_CONSOLE_UART1
	imx_iomux_v3_setup_multiple_pads(uart1_pads, ARRAY_SIZE(uart1_pads));
#elif defined CONFIG_CONSOLE_UART3
	imx_iomux_v3_setup_multiple_pads(uart3_pads, ARRAY_SIZE(uart3_pads));
#endif
}

int board_early_init_f(void)
{
	setup_iomux_uart();
	return 0;
}
