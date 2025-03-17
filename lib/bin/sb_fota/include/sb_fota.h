/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file sb_fota.h
 *
 * @brief Public APIs for the SoftBank FOTA client.
 * @defgroup sb_fota SoftBank FOTA client
 * @{
 */

#ifndef SB_FOTA_H_
#define SB_FOTA_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SoftBank FOTA application events.
 */
enum sb_fota_event {
	/** FOTA client starts networking.
	 * Application should close all TLS sockets. Modem may change the operation mode
	 * from NB-IOT to LTE-M, so any network activity is recommended to be paused.
	 */
	SB_FOTA_EVENT_DOWNLOADING,
	/** FOTA client is now on idle.
	 * Application may resume normal network operations.
	 */
	SB_FOTA_EVENT_IDLE,
	/** Modem FW is going to be updated, so it is shut down.
	 * Application cannot use the network anymore until firmware is updated and
	 * device is booted.
	 */
	SB_FOTA_EVENT_MODEM_SHUTDOWN,
};

/**
 * @brief SoftBank FOTA application callback function.
 *
 * Also see @ref sb_fota_event.
 *
 * @param event Event ID.
 */
typedef void (*sb_fota_callback_t)(enum sb_fota_event event);

/**
 * @brief Initializes the SoftBank FOTA client.
 *
 * Initializes the library and registers a callback for FOTA process.
 * Subsequent calls may be used to change the callback function.
 *
 * If Kconfig value CONFIG_SB_FOTA_AUTOINIT is set, then application is not required to call this
 * function, but might still want to use it for registering the callback.
 *
 * @param callback Callback for the application events or NULL for no callback.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int sb_fota_init(sb_fota_callback_t callback);

/**
 * @brief SoftBank FOTA modem functional modes.
 *
 * Used to pass on the modem functional mode in @ref sb_fota_on_modem_cfun().
 * @{
 */
/** Minimum functionality mode. */
#define SB_FOTA_MODEM_MODE_POWER_OFF   0
/** Any valid functionality mode other than @c SB_FOTA_MODEM_MODE_POWER_OFF. */
#define SB_FOTA_MODEM_MODE_POWER_ON    1
/** @} */

/**
 * SoftBank FOTA library modem functional mode handler.
 *
 * @param[in] mode Modem functional mode.
 *
 * This function must be called whenever modem functional mode is changed.
 *
 * @note This is already implemented in `sb_fota_os.c`.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int sb_fota_on_modem_cfun(int mode);

/**
 * @brief Sets current time for the SoftBank FOTA client and modem clock.
 *
 * Sets the current time used by the SoftBank FOTA client. The given time is also
 * set to the modem. If LTE network time is not available, current time has
 * to be provided to the SoftBank FOTA client using this function.
 *
 * LTE network time overrides the time set using this function. If time is
 * available from the LTE network, time doesn't have to be set using this
 * function.
 *
 * @param time_str Time as a null terminated string in format
 *                 "yy/MM/dd,hh:mm:ss±zz", where the characters, from left to
 *                 right, indicate year, month, day, hour, minutes, seconds and
 *                 time zone. Time zone indicates the difference, expressed in
 *                 quarters of an hour, between the local time and GMT (value
 *                 range -48...+48). For example "20/10/15,09:12:47+12".
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int sb_fota_clock_set(const char *time_str);

#ifdef __cplusplus
}
#endif

#endif /* SB_FOTA_H_ */

/** @} */
