/*
 * Board functions for Atmark Techno Armadillo-640
 *
 * Copyright (C) 2017 Yasushi SHOJI <yashi@atmark-techno.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#ifdef CONFIG_USB_EHCI_MX6
int setup_usb(void);
#else
int setup_usb(void) {return 0;}
#endif
