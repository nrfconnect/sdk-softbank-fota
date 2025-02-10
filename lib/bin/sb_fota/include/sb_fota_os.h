/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
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
 * @brief Allocate memory.
 */
void *sb_fota_os_malloc(size_t size);

/**
 * @brief Allocate memory and zero it.
 */
void *sb_fota_os_calloc(size_t nmemb, size_t size);

/**
 * @brief Free memory.
 */
void sb_fota_os_free(void *ptr);

/**
 * @brief Get uptime, in milliseconds.
 */
int64_t sb_fota_os_uptime_get(void);

/**
 * @brief Get uptime, in milliseconds.
 */
uint32_t sb_fota_os_uptime_get_32(void);

/**
 * @brief Put a thread to a sleep.
 */
int sb_fota_os_sleep(int ms);

/**
 * @brief Reboot system.
 */
void sb_fota_os_sys_reset(void);

/**
 * @brief Get a random value.
 */
uint32_t sb_fota_os_rand_get(void);

#define SB_FOTA_OS_SEMAPHORE_COUNT 4

/** Opaque type for OS semaphore */
struct sb_fota_os_sem;

/**
 * @brief Allocate a semaphore from OS
 *
 * @return pointer to allocated semaphore or NULL on failure.
 */
struct sb_fota_os_sem *sb_fota_os_sem_alloc(void);

/**
 * @brief Give a semaphore.
 *
 * @param sem pointer to a taken semaphore.
 */
void sb_fota_os_sem_give(struct sb_fota_os_sem *sem);

/**
 * @brief Take a semaphore.
 *
 * @param sem pointer to semaphore
 * @param timeout_ms timeout in milliseconds, or negative if call can block forever.
 * @return zero on success or negative error code on failure.
 */
int sb_fota_os_sem_take(struct sb_fota_os_sem *sem, int timeout_ms);

/**
 * @brief Reset semaphore count to zero.
 *
 * @param sem pointer to a semaphore.
 */
void sb_fota_os_sem_reset(struct sb_fota_os_sem *sem);

#define SB_FOTA_OS_WORK_COUNT 6
#define SB_FOTA_OS_DELAYED_WORK_COUNT 1

/** Generic work callback type */
typedef void (*sb_fota_os_work_cb)(void *);

/** Opaque work type */
struct sb_fota_os_work;

/** Opaque delayed work type */
struct sb_fota_os_delayed_work;

struct sb_fota_os_work *sb_fota_os_work_init(sb_fota_os_work_cb cb);
void sb_fota_os_work_schedule(struct sb_fota_os_work *work);
struct sb_fota_os_delayed_work *sb_fota_os_delayed_work_init(sb_fota_os_work_cb cb);
void sb_fota_os_delayed_work_schedule(struct sb_fota_os_delayed_work *work, int delay_ms);

#define SB_FOTA_OS_TIMERS 2

/** Opaque timer type */
struct sb_fota_os_timer;

struct sb_fota_os_timer *sb_fota_os_timer_init(sb_fota_os_work_cb cb);
void sb_fota_os_timer_start(struct sb_fota_os_timer *timer, uint64_t delay_ms);
void sb_fota_os_timer_stop(struct sb_fota_os_timer *timer);
bool sb_fota_os_timer_is_running(struct sb_fota_os_timer *timer);

int64_t sb_fota_os_timegm64(const struct tm *time);

#define FOTA_LOG_LEVEL_NONE 0U
#define FOTA_LOG_LEVEL_ERR  1U
#define FOTA_LOG_LEVEL_WRN  2U
#define FOTA_LOG_LEVEL_INF  3U
#define FOTA_LOG_LEVEL_DBG  4U

void sb_fota_os_log(int level, const char *fmt, ...);
const char *sb_fota_os_log_strdup(const char *str);

#define FOTA_LOG_ERR(...) sb_fota_os_log(FOTA_LOG_LEVEL_ERR, __VA_ARGS__);
#define FOTA_LOG_WRN(...) sb_fota_os_log(FOTA_LOG_LEVEL_WRN, __VA_ARGS__);
#define FOTA_LOG_INF(...) sb_fota_os_log(FOTA_LOG_LEVEL_INF, __VA_ARGS__);
#define FOTA_LOG_DBG(...) sb_fota_os_log(FOTA_LOG_LEVEL_DBG, __VA_ARGS__);

#define SB_FOTA_SETTINGS_PREFIX "sb_fota"

/** Structure used to load settings from persistent storage */
struct sb_fota_settings {
	const char *name; /** Name of the setting */
	size_t len;       /** Size of data, or zero for variable lenght data like strings */
	void *ptr;	  /** Pointer to runtime storage. Strings are allocated and ptr stored here. */
};

/** Load array of settings from persistent storage.
 *  Settings should be given as an array that ends with element where name is NULL.
 */
void sb_fota_os_load_settings(const struct sb_fota_settings *settings);

/** Store one settings value.
 * SB_FOTA_SETTINGS_PREFIX is automatically added to the name.
 */
void sb_fota_os_store_setting(const char *name, size_t len, const void *ptr);

/** Apply modem FW update */
void sb_fota_os_update_apply(void);

/** @} */

#ifdef UNITTESTING
/* For unittesting purposes, I need dummy types of every opaque struct */
struct sb_fota_os_sem
{
	int val;
};
struct sb_fota_os_work
{
	int val;
};
struct sb_fota_os_delayed_work
{
	int val;
};
struct sb_fota_os_timer
{
	int val;
};
#endif

/* Enums */
enum modem_reg_status {
	MODEM_REG_STATUS_NOT_REGISTERED,
	MODEM_REG_STATUS_HOME,
	MODEM_REG_STATUS_ROAMING
};

enum modem_lte_mode {
	MODEM_LTE_MODE_M = 7,
	MODEM_LTE_MODE_NBIOT = 9
};

#ifdef CONFIG_SB_FOTA_INCLUDE_DEBUG_INFORMATION
int get_lte_mode(void);
bool get_lte_active_status(void);
bool get_rrc_idle_status(void);
int get_reg_status(void);
struct sb_fota_os_timer *get_active_timer(void);
struct sb_fota_os_work *get_work(void);
#endif

#endif /* SB_FOTA_OS_H_ */
