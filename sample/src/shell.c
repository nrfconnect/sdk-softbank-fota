/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/shell/shell.h>
#include <sb_fota.h>

static int app_cmd_clock(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	int err;

	err = sb_fota_clock_set(argv[1]);
	if (err) {
		shell_error(shell, "clock: invalid time string");
		return -EINVAL;
	}

	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
	app_cmds,
	SHELL_CMD_ARG(clock, NULL,
		      "Set modem clock (see modem AT command specification for AT+CCLK for "
		      "detailed format description)\n"
		      "Usage:\n"
		      "app clock <yy/MM/dd,hh:mm:ss-/+zz>",
		      app_cmd_clock, 2, 0),
);

SHELL_CMD_REGISTER(app, &app_cmds, "Commands for controlling the SoftBank FOTA sample application", NULL);
