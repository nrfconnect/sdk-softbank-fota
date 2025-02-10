/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include "sb_fota_settings.h"
#include "sb_fota_os.h"
#include <stdbool.h>
#include <string.h>
#include <modem/modem_attest_token.h>
#include <modem/modem_jwt.h>
#include <modem/modem_info.h>
#include <zephyr/sys/util_macro.h>

#define IMEI_LEN 15

int sb_fota_settings_cloud_sec_tag_get(void)
{
	return CONFIG_SB_FOTA_TLS_SECURITY_TAG;
}

int sb_fota_settings_jwt_sec_tag_get(void)
{
	return CONFIG_SB_FOTA_JWT_SECURITY_TAG;
}

static char sb_fota_client_id[NRF_DEVICE_UUID_STR_LEN + sizeof(CONFIG_SB_FOTA_ID_PREFIX)];

static int get_uuid(void)
{
	int ret;
	static struct nrf_device_uuid uuid;

	sb_fota_client_id[0] = '\0';
	ret = modem_jwt_get_uuids(&uuid, NULL);
	if (ret) {
		return ret;
	}

	strcat(sb_fota_client_id, CONFIG_SB_FOTA_ID_PREFIX);
	strcat(sb_fota_client_id, uuid.str);
	sb_fota_client_id[sizeof(sb_fota_client_id) - 1] = '\0';

	return 0;
}

static int get_imei(void)
{
	int ret;
	char imei[IMEI_LEN + sizeof("\r\nOK\r\n")];

	sb_fota_client_id[0] = '\0';
	ret = modem_info_string_get(MODEM_INFO_IMEI, imei, sizeof(imei));
	if (ret < IMEI_LEN) {
		return ret;
	}

	strcat(sb_fota_client_id, CONFIG_SB_FOTA_ID_PREFIX);
	strcat(sb_fota_client_id, imei);
	sb_fota_client_id[sizeof(sb_fota_client_id) - 1] = '\0';

	return 0;
}

const char *sb_fota_settings_client_id_get(void)
{
	bool initialized = sb_fota_client_id[0] != '\0';

	if (!initialized) {
		int ret;
		if (IS_ENABLED(CONFIG_SB_FOTA_ID_UUID)) {
			if ((ret = get_uuid())) {
				FOTA_LOG_ERR("Failed to read UUID (err %d)", ret);
			}
		} else {
			if ((ret = get_imei())) {
				FOTA_LOG_ERR("Failed to read IMEI (err %d)", ret);
			}
		}
	}

	return sb_fota_client_id;
}

void sb_fota_settings_client_id_set(const char *id)
{
	strncpy(sb_fota_client_id, id, sizeof(sb_fota_client_id) - 1);
	sb_fota_client_id[sizeof(sb_fota_client_id) - 1] = '\0';
}
