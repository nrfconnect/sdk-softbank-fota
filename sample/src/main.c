/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <stdio.h>
#include <string.h>
#include <nrf_modem.h>
#include <sb_fota.h>
#include <modem/nrf_modem_lib.h>
#include <modem/lte_lc.h>

NRF_MODEM_LIB_ON_DFU_RES(sb_fota_dfu_hook, on_modem_lib_dfu, NULL);

static void on_modem_lib_dfu(int32_t dfu_res, void *ctx)
{
	ARG_UNUSED(ctx);

	switch (dfu_res) {
	case NRF_MODEM_DFU_RESULT_OK:
		printk("Modem firmware update successful.\n");
		printk("Modem is running the new firmware.\n");
		break;
	case NRF_MODEM_DFU_RESULT_HARDWARE_ERROR:
	case NRF_MODEM_DFU_RESULT_INTERNAL_ERROR:
		printk("Modem firmware update failed.\n");
		printk("Fatal error.\n");
		break;
	case NRF_MODEM_DFU_RESULT_UUID_ERROR:
	case NRF_MODEM_DFU_RESULT_AUTH_ERROR:
	default:
		printk("Modem firmware update failed.\n");
		printk("Modem is running non-updated firmware.\n");
		break;
	}
}

static void sb_fota_cb(enum sb_fota_event e)
{
	switch(e) {
	case SB_FOTA_EVENT_DOWNLOADING:
		printk("SoftBank FOTA library: Checking for update\n");
		break;
	case SB_FOTA_EVENT_IDLE:
		printk("SoftBank FOTA library: Idle\n");
		break;
	case SB_FOTA_EVENT_MODEM_SHUTDOWN:
		printk("SoftBank FOTA library: Update downloaded. Modem will be updated.\n");
		break;
	}
}

int main(void)
{
	int err;

	printk("SoftBank FOTA client started\n");

	err = nrf_modem_lib_init();
	if (err) {
		printk("Failed to initialize the modem library, err %d\n", err);
		return err;
	}

	if (sb_fota_init(&sb_fota_cb) != 0) {
		printk("Failed to initialize the SoftBank FOTA library\n");
		return 1;
	}

	err = lte_lc_connect();
	if (err) {
		printk("Connecting to network failed, err %d\n", err);
		return err;
	}

	k_sleep(K_FOREVER);
}
