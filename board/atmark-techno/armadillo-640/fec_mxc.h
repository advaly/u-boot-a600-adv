/*
 * Board functions for Atmark Techno Armadillo-640
 *
 * Copyright (C) 2017 Yasushi SHOJI <yashi@atmark-techno.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#ifdef CONFIG_FEC_MXC
int setup_fec(void);
#else
int setup_fec(void) {return 0;}
#endif
