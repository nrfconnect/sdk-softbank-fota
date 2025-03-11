/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file sb_fota_settings.h
 *
 * @defgroup sb_fota_settings Softbank FOTA build time settings
 * @{
 */

#ifndef SB_FOTA_SETTINGS_H
#define SB_FOTA_SETTINGS_H

#define SB_FOTA_CLIENT_ID_LEN  (NRF_DEVICE_UUID_STR_LEN + sizeof(CONFIG_SB_FOTA_ID_PREFIX))

/**
 * @brief Get sec_tag for cloud TLS communication.
 *
 * @return sec_tag
 */
int sb_fota_settings_cloud_sec_tag_get(void);

/**
 * @brief Get sec_tag for generating JWT tokens.
 *
 * @return sec_tag
 */
int sb_fota_settings_jwt_sec_tag_get(void);

/**
 * @brief Get client identifier.
 *
 * @return client identifier string
 */
const char *sb_fota_settings_client_id_get(void);

/**
 * @brief Set client identifier for FOTA library.
 *
 * If not set, default IMEI or UUID is used depending on the Kconfig option.
 *
 * @param id Client identifier string, maximum @c NRF_DEVICE_UUID_STR_LEN characters.
 */
void sb_fota_settings_client_id_set(const char *id);

/** @} */

#endif /* SB_FOTA_SETTINGS_H */
