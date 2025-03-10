/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file sb_fota_os.h
 *
 * @defgroup sb_fota_os Softbank FOTA OS layer
 * @{
 */

#ifndef SB_FOTA_OS_H_
#define SB_FOTA_OS_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/**
 * @brief Allocate memory on the heap.
 *
 * @param[in] size Size of the memory to be allocated.
 *
 * @return A pointer to the allocated memory buffer or @c NULL if allocation failed.
 */
void *sb_fota_os_malloc(size_t size);

/**
 * @brief Allocate memory on the heap and zero it.
 *
 * @param[in] nmemb Number of elements in the requested array
 * @param[in] size  Size of each array element.
 *
 * @return A pointer to the allocated memory buffer or @c NULL if allocation failed.
 */
void *sb_fota_os_calloc(size_t nmemb, size_t size);

/**
 * @brief Free memory allocated on the heap
 *
 * @param[in] ptr Pointer to the memory to be freed.
 */
void sb_fota_os_free(void *ptr);

/**
 * @brief Get system uptime, in milliseconds.
 *
 * @return Uptime in milliseconds.
 */
int64_t sb_fota_os_uptime_get(void);

/**
 * @brief Get system uptime (32-bit version), in milliseconds.
 *
 * @return Uptime in milliseconds.
 */
uint32_t sb_fota_os_uptime_get_32(void);

/**
 * @brief Convert broken-down time to a POSIX epoch offset in seconds.
 *
 * @param[in] time Pointer to broken down time.
 *
 * @return Time converted to the POSIX epoch time scale.
 */
int64_t sb_fota_os_timegm64(const struct tm *time);

/**
 * @brief Put a thread to sleep for a specific amount of time.
 *
 * @param[in] ms Timeout in milliseconds.
 *
 * @retval zero on success or negative error code on failure.
 */
int sb_fota_os_sleep(int ms);

/**
 * @brief Get a random value.
 *
 * @return 32-bit random value.
 */
uint32_t sb_fota_os_rand_get(void);

/** Generic type for OS semaphore. */
typedef int sb_fota_os_sem_t;

/**
 * @brief Allocate a semaphore from OS.
 *
 * @return pointer to the allocated semaphore or NULL on failure.
 */
sb_fota_os_sem_t *sb_fota_os_sem_alloc(void);

/**
 * @brief Give a semaphore.
 *
 * @param[in] sem Pointer to the taken semaphore.
 */
void sb_fota_os_sem_give(sb_fota_os_sem_t *sem);

/**
 * @brief Take a semaphore.
 *
 * @param[in] sem        Pointer to the semaphore
 * @param[in] timeout_ms Timeout in milliseconds, or negative on infinite timeout.
 *
 * @retval zero on success or negative error code on failure.
 */
int sb_fota_os_sem_take(sb_fota_os_sem_t *sem, int timeout_ms);

/**
 * @brief Reset semaphore count to zero.
 *
 * @param[in] sem Pointer to the semaphore.
 */
void sb_fota_os_sem_reset(sb_fota_os_sem_t *sem);

/** Generic work callback type. */
typedef void (*sb_fota_os_work_cb)(void *);

/** Generic work type. */
typedef int sb_fota_os_work_t;

/** Generic delayed work type. */
typedef int sb_fota_os_delayed_work_t;

/**
 * @brief Initialize the next available work structure.
 *
 * @param[in] cb The handler to be invoked by the work item.
 *
 * @return pointer to the initialized work structure or NULL if no work items available.
 */
sb_fota_os_work_t *sb_fota_os_work_init(sb_fota_os_work_cb cb);

/**
 * @brief Submit a work item on the library's work queue.
 *
 * @param[in] work Work item to be submitted.
 */
void sb_fota_os_work_schedule(sb_fota_os_work_t *work);

/**
 * @brief Initialize the next available delayed work structure.
 *
 * @param[in] cb The handler to be invoked by the delayed work item.
 *
 * @return pointer to the initialized delayed work structure or NULL if no work items available.
 */
sb_fota_os_delayed_work_t *sb_fota_os_delayed_work_init(sb_fota_os_work_cb cb);

/**
 * @brief Schedule a work item on the library's work queue.
 *
 * @param[in] work     Work item to be scheduled.
 * @param[in] delay_ms Delay before submitting the task in milliseconds.
 */
void sb_fota_os_delayed_work_schedule(sb_fota_os_delayed_work_t *work, int delay_ms);

/** Generic timer type. */
typedef int sb_fota_os_timer_t;

/**
 * @brief Initialize a timer.
 *
 * @param[in] cb Callback to be invoked each time the timer expires.
 *
 * @return Pointer to the initialized timer.
 */
sb_fota_os_timer_t *sb_fota_os_timer_init(sb_fota_os_work_cb cb);

/**
 * @brief Start a timer.
 *
 * @param[in] timer    Pointer to the timer to be started.
 * @param[in] delay_ms Timer duration in milliseconds.
 */
void sb_fota_os_timer_start(sb_fota_os_timer_t *timer, uint64_t delay_ms);

/**
 * @brief Stop a timer.
 *
 * @param[in] timer Pointer to the timer to be stopped.
 */
void sb_fota_os_timer_stop(sb_fota_os_timer_t *timer);

/**
 * @brief Check if a timer is running.
 *
 * @param[in] timer Pointer to the timer.
 *
 * @retval true if the timer is running, false otherwise.
 */
bool sb_fota_os_timer_is_running(sb_fota_os_timer_t *timer);

/** Generic logging levels. */
enum sb_fota_os_log_level {
	SB_FOTA_OS_LOG_LEVEL_NONE,
	SB_FOTA_OS_LOG_LEVEL_ERR,
	SB_FOTA_OS_LOG_LEVEL_WRN,
	SB_FOTA_OS_LOG_LEVEL_INF,
	SB_FOTA_OS_LOG_LEVEL_DBG,
};

/**
 * @brief Generic logging procedure.
 *
 * @param level Log level.
 * @param fmt Format string.
 * @param ... Varargs.
 */
void sb_fota_os_log(int level, const char *fmt, ...);

#define SB_FOTA_SETTINGS_PREFIX "sb_fota"

/** Structure used to load settings from persistent storage. */
struct sb_fota_settings {
	const char *name; /** Name of the setting */
	size_t len;       /** Size of data, or zero for variable lenght data like strings */
	void *ptr;	  /** Pointer to runtime storage. Strings are allocated and ptr stored here. */
};

/**
 * @brief Load the array of settings from persistent storage.
 *
 * Settings must be given as an array that ends with an element whose name is NULL.
 *
 * @param[in] settings Array to load stored settings into.
 */
void sb_fota_os_load_settings(const struct sb_fota_settings *settings);

/**
 * @brief Store a setting value in persistent storage.
 *
 * @c SB_FOTA_SETTINGS_PREFIX is automatically added to the name.
 *
 * @param[in] name Null-terminated name of the setting to be stored.
 * @param[in] len  Length of the setting value to be stored.
 * @param[in] ptr  Value of the setting to be stored.
 */
void sb_fota_os_store_setting(const char *name, size_t len, const void *ptr);

/**
 * @brief Apply the modem firmware update and restore the LTE connection.
 */
void sb_fota_os_update_apply(void);

/**
 * @brief Activate the LTE service in the modem.
 *
 * @retval zero on success or negative error code on failure.
 */
int sb_fota_os_lte_online(void);

/**
 * @brief Deactivate the LTE service in the modem.
 *
 * @retval zero on success or negative error code on failure.
 */
int sb_fota_os_lte_offline(void);

/**
 * @brief Request to set LTE-M (Cat-M1) as the preferred bearer in the modem.
 *
 * @retval zero on success or negative error code on failure.
 */
int sb_fota_os_lte_mode_request_ltem(void);

/**
 * @brief Request to restore the application-preferred bearer in the modem.
 *
 * @retval zero on success or negative error code on failure.
 */
int sb_fota_os_lte_mode_restore(void);

/** @} */

#endif /* SB_FOTA_OS_H_ */
