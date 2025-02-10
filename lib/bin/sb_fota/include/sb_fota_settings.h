/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

/**
 * @file sb_fota_settings.h
 *
 * @defgroup sb_fota_settings Softbank modem FOTA build time settings
 * @{
 */

#ifndef SB_FOTA_SETTINGS_H
#define SB_FOTA_SETTINGS_H


/**
 * @brief Get sec_tag for cloud TLS communication.
 *
 * @return sec_tag
 */
int sb_fota_cloud_sec_tag(void);

/**
 * @brief Get sec_tag for generating JWT tokens.
 *
 * @return sec_tag
 */
int sb_fota_jwt_sec_tag(void);

/**
 * @brief Get client identifier
 *
 * @return client identifier string
 */
const char *sb_fota_get_client_id(void);

/** @} */

#endif /* SB_FOTA_SETTINGS_H */
