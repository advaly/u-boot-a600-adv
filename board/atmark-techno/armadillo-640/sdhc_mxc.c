/*
 * Board functions for Atmark Techno Armadillo-640
 *
 * Copyright (C) 2017 Hiroaki OHSAWA <ohsawa@atmark-techno.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#include <asm/arch/mx6-pins.h>
#include <asm/arch/iomux.h>
#include <asm/arch/clock.h>
#include <mmc.h>
#include <fsl_esdhc.h>

/* We use internal pull-ups for SD1 / on-board eMMC */
#define SD1_CLK_PAD_CTRL   (                                   PAD_CTL_SPEED_MED | PAD_CTL_DSE_48ohm | PAD_CTL_SRE_FAST)
#define SD1_CMD_PAD_CTRL   (PAD_CTL_HYS | PAD_CTL_PUS_47K_UP | PAD_CTL_SPEED_MED | PAD_CTL_DSE_80ohm | PAD_CTL_SRE_FAST)
#define SD1_DATA_PAD_CTRL  (PAD_CTL_HYS | PAD_CTL_PUS_47K_UP | PAD_CTL_SPEED_MED | PAD_CTL_DSE_80ohm | PAD_CTL_SRE_FAST)
#define SD1_RESET_PAD_CTRL (                                                       PAD_CTL_DSE_240ohm                  )

/* We have external pull-ups for SD2 / microSD. Thus, no internal pull-ups */
#define SD2_CLK_PAD_CTRL   (                                   PAD_CTL_SPEED_MED | PAD_CTL_DSE_48ohm  | PAD_CTL_SRE_FAST)
#define SD2_CMD_PAD_CTRL   (PAD_CTL_HYS |                      PAD_CTL_SPEED_MED | PAD_CTL_DSE_80ohm  | PAD_CTL_SRE_FAST)
#define SD2_DATA_PAD_CTRL  (PAD_CTL_HYS |                      PAD_CTL_SPEED_MED | PAD_CTL_DSE_80ohm  | PAD_CTL_SRE_FAST)
#define SD2_RESET_PAD_CTRL (                                                       PAD_CTL_DSE_240ohm                   )

/* SD1 is connected to on-board eMMC */
static iomux_v3_cfg_t const sd1_pads[] = {
	MX6_PAD_SD1_CLK__USDHC1_CLK        | MUX_PAD_CTRL(SD1_CLK_PAD_CTRL),
	MX6_PAD_SD1_CMD__USDHC1_CMD        | MUX_PAD_CTRL(SD1_CMD_PAD_CTRL),
	MX6_PAD_SD1_DATA0__USDHC1_DATA0    | MUX_PAD_CTRL(SD1_DATA_PAD_CTRL),
	MX6_PAD_SD1_DATA1__USDHC1_DATA1    | MUX_PAD_CTRL(SD1_DATA_PAD_CTRL),
	MX6_PAD_SD1_DATA2__USDHC1_DATA2    | MUX_PAD_CTRL(SD1_DATA_PAD_CTRL),
	MX6_PAD_SD1_DATA3__USDHC1_DATA3    | MUX_PAD_CTRL(SD1_DATA_PAD_CTRL),
	MX6_PAD_NAND_READY_B__USDHC1_DATA4 | MUX_PAD_CTRL(SD1_DATA_PAD_CTRL),
	MX6_PAD_NAND_CE0_B__USDHC1_DATA5   | MUX_PAD_CTRL(SD1_DATA_PAD_CTRL),
	MX6_PAD_NAND_CE1_B__USDHC1_DATA6   | MUX_PAD_CTRL(SD1_DATA_PAD_CTRL),
	MX6_PAD_NAND_CLE__USDHC1_DATA7     | MUX_PAD_CTRL(SD1_DATA_PAD_CTRL),
	MX6_PAD_GPIO1_IO09__USDHC1_RESET_B | MUX_PAD_CTRL(SD1_RESET_PAD_CTRL),
};

/* SD2 is for microSD */
static iomux_v3_cfg_t const sd2_pads[] = {
	MX6_PAD_NAND_RE_B__USDHC2_CLK     | MUX_PAD_CTRL(SD2_CLK_PAD_CTRL),
	MX6_PAD_NAND_WE_B__USDHC2_CMD     | MUX_PAD_CTRL(SD2_CMD_PAD_CTRL),
	MX6_PAD_NAND_DATA00__USDHC2_DATA0 | MUX_PAD_CTRL(SD2_DATA_PAD_CTRL),
	MX6_PAD_NAND_DATA01__USDHC2_DATA1 | MUX_PAD_CTRL(SD2_DATA_PAD_CTRL),
	MX6_PAD_NAND_DATA02__USDHC2_DATA2 | MUX_PAD_CTRL(SD2_DATA_PAD_CTRL),
	MX6_PAD_NAND_DATA03__USDHC2_DATA3 | MUX_PAD_CTRL(SD2_DATA_PAD_CTRL),
	MX6_PAD_NAND_ALE__USDHC2_RESET_B  | MUX_PAD_CTRL(SD2_RESET_PAD_CTRL),
};

static void mux_sdhc(void)
{

	imx_iomux_v3_setup_multiple_pads(sd1_pads,
					 ARRAY_SIZE(sd1_pads));
	imx_iomux_v3_setup_multiple_pads(sd2_pads,
					 ARRAY_SIZE(sd2_pads));
}

static struct fsl_esdhc_cfg usdhc_cfg[] = {
	[0] = {
		.esdhc_base = USDHC1_BASE_ADDR,
		.max_bus_width = 8,
		.wp_enable = 0,
		.vs18_enable = 0,
	},
	[1] = {
		.esdhc_base = USDHC2_BASE_ADDR,
		.max_bus_width = 4,
		.wp_enable = 0,
		.vs18_enable = 0,
	}
};

int board_mmc_getcd(struct mmc *mmc)
{
	return 1;
}

int board_mmc_init(bd_t *bis)
{
	int ret;

	mux_sdhc();

	usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC_CLK);
	ret = fsl_esdhc_initialize(bis, &usdhc_cfg[0]);
	if (ret)
		pr_err("fsl_esdhc_initialize() failed with %d for on-board eMMC\n", ret);

	usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC2_CLK);
	ret = fsl_esdhc_initialize(bis, &usdhc_cfg[1]);
	if (ret)
		pr_err("fsl_esdhc_initialize() failed with %d for microSD\n", ret);

	/* we don't want to call cpu_mmc_init() which in turn calls
	 * fsl_esdhc_mmc_init() for on-board eMMC again, so always
	 * return zero. see mmc_probe(). */
	return 0;
}
