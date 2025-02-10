/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include "sb_fota_os.h"
#include "sb_fota.h"
#include <stdio.h>
#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/types.h>
#include <zephyr/random/random.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/sys/timeutil.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_msg.h>
#include <zephyr/settings/settings.h>
#include <modem/nrf_modem_lib.h>
#include <modem/lte_lc.h>
#include <fota_download_util.h>

/* Memory management. */

void *modem_fota_os_malloc(size_t size)
{
	return k_malloc(size);
}

void *modem_fota_os_calloc(size_t nmemb, size_t size)
{
	return k_calloc(nmemb, size);
}

void modem_fota_os_free(void *ptr)
{
	k_free(ptr);
}

/* Timing functions. */

int64_t modem_fota_os_uptime_get(void)
{
	return k_uptime_get();
}

uint32_t modem_fota_os_uptime_get_32(void)
{
	return k_uptime_get_32();
}

int modem_fota_os_sleep(int ms)
{
	return k_sleep(K_MSEC(ms));
}

/* OS functions */

void modem_fota_os_sys_reset(void)
{
	sys_reboot(SYS_REBOOT_COLD);
	CODE_UNREACHABLE;
}

uint32_t modem_fota_os_rand_get(void)
{
	return sys_rand32_get();
}

/* Semaphores */

struct modem_fota_sem {
	struct k_sem sem;
};

static struct modem_fota_sem semaphores[MODEM_FOTA_N_SEMAPHORES];

struct modem_fota_sem *modem_fota_sem_alloc() {
	static int next_free;

	if (next_free >= MODEM_FOTA_N_SEMAPHORES) {
		return NULL;
	}

	struct modem_fota_sem *sem = &semaphores[next_free++];
	k_sem_init(&sem->sem, 0, 1);
	return sem;
}

void modem_fota_sem_give(struct modem_fota_sem *sem)
{
	k_sem_give(&sem->sem);
}

int modem_fota_sem_take(struct modem_fota_sem *sem, int timeout_ms)
{
	k_timeout_t t;
	if (timeout_ms == 0) {
		t = K_NO_WAIT;
	} else if (timeout_ms < 0) {
		t = K_FOREVER;
	} else {
		t = K_MSEC(timeout_ms);
	}
	return k_sem_take(&sem->sem, t);
}

void modem_fota_sem_reset(struct modem_fota_sem *sem)
{
	k_sem_reset(&sem->sem);
}

/* Work queue */
#define MY_PRIORITY 5

K_THREAD_STACK_DEFINE(modem_fota_stack_area, CONFIG_MAIN_STACK_SIZE);

static struct k_work_q modem_fota_work_q;
static bool q_is_initialized = false;

struct modem_fota_event {
	struct k_work w;
};
struct modem_fota_delayed_event {
	struct k_work_delayable dw;
};

static struct modem_fota_event events[MODEM_FOTA_N_EVENTS];
static struct modem_fota_delayed_event delayed_events[MODEM_FOTA_N_DELAYED_EVENTS];

struct modem_fota_event *modem_fota_event_init(void (*callback)(void *))
{
	static int next_free;

	if (!q_is_initialized) {
		k_work_queue_init(&modem_fota_work_q);
		k_work_queue_start(&modem_fota_work_q, modem_fota_stack_area,
				   K_THREAD_STACK_SIZEOF(modem_fota_stack_area), MY_PRIORITY, NULL);
		k_thread_name_set(&modem_fota_work_q.thread, "sb_fota");
		q_is_initialized = true;
	}

	if (next_free >= MODEM_FOTA_N_EVENTS) {
		return NULL;
	}

	struct modem_fota_event *evt = &events[next_free++];
	k_work_init(&evt->w, (void (*)(struct k_work *)) callback);
	return evt;
}

void modem_fota_event_send(struct modem_fota_event *evt)
{
	k_work_submit_to_queue(&modem_fota_work_q, & evt->w);
}

struct modem_fota_delayed_event *modem_fota_delayed_event_init(void (*callback)(void *))
{
	static int next_free;

	if (next_free >= MODEM_FOTA_N_DELAYED_EVENTS) {
		return NULL;
	}

	struct modem_fota_delayed_event *evt = &delayed_events[next_free++];
	k_work_init_delayable(&evt->dw, (void (*)(struct k_work *)) callback);
	return evt;
}

void modem_fota_delayed_event_send(struct modem_fota_delayed_event *evt, int delay_ms)
{
	k_work_schedule_for_queue(&modem_fota_work_q, &evt->dw, K_MSEC(delay_ms));
}

/* Timers */

struct modem_fota_timer {
	struct k_timer t;
};

struct modem_fota_timer timers[MODEM_FOTA_N_TIMERS];

struct modem_fota_timer *modem_fota_timer_init(void (*callback)(void *))
{
	static int next_free;

	if (next_free >= MODEM_FOTA_N_TIMERS) {
		return NULL;
	}

	struct modem_fota_timer *timer = &timers[next_free++];
	k_timer_init(&timer->t, (void (*)(struct k_timer *))callback, NULL);
	return timer;
}

void modem_fota_timer_start(struct modem_fota_timer *timer, uint64_t delay_ms)
{
	k_timeout_t timeout = ((delay_ms) ? K_MSEC(delay_ms) : K_NO_WAIT);
	k_timer_start(&timer->t, timeout, K_NO_WAIT);
}

void modem_fota_timer_stop(struct modem_fota_timer *timer)
{
	k_timer_stop(&timer->t);
}

bool modem_fota_timer_is_running(struct modem_fota_timer *timer)
{
	return k_timer_remaining_get(&timer->t) != 0;
}

int64_t modem_fota_timegm64(const struct tm *time)
{
	return timeutil_timegm64(time);
}

/* Logging */

LOG_MODULE_REGISTER(sb_fota, CONFIG_SB_FOTA_LOG_LEVEL);

void modem_fota_log(int level, const char *fmt, ...)
{
	if (!IS_ENABLED(CONFIG_LOG_MODE_MINIMAL)) {
		va_list ap;

		va_start(ap, fmt);
		log_generic(level, fmt, ap);
		va_end(ap);
	}
}

const char *modem_fota_log_strdup(const char *str)
{
	return str;
}

void modem_fota_logdump(const char *str, const void *data, size_t len)
{
	if (IS_ENABLED(CONFIG_LOG)) {
		LOG_HEXDUMP_DBG(data, len, str);
	}
}

/* Settings */

static const struct modem_fota_settings *modem_fota_settings;

static int settings_set(const char *name, size_t len,
		settings_read_cb read_cb, void *cb_arg)
{
	void *data;

	for (const struct modem_fota_settings *sp = modem_fota_settings; sp->name; ++sp) {
		if (strcmp(name, sp->name)) {
			continue;
		}
		/* Do I need to allocate the storage? */
		if (sp->len > 0) {
			data = sp->ptr;
		} else {
			data = modem_fota_os_malloc(len);
			*((void**)sp->ptr) = data;
			if (data == NULL) {
				return -ENOMEM;
			}
		}

		if (read_cb(cb_arg, data, len) > 0) {
			return 0;
		} else {
			/* if this was allocated, free it */
			if (sp->len == 0) {
				modem_fota_os_free(data);
				*((void**)sp->ptr) = NULL;
			}
			return -EINVAL;
		}
	}
	return -ENOENT;
}

void modem_fota_load_settings(const struct modem_fota_settings *settings)
{
	modem_fota_settings = settings;

	settings_subsys_init();

	struct settings_handler my_conf = {
	    .name = MODEM_FOTA_SETTINGS_PREFIX,
	    .h_set = settings_set
	};

	settings_register(&my_conf);
	settings_load_subtree(MODEM_FOTA_SETTINGS_PREFIX);
}


void modem_fota_store_setting(const char *name, size_t len, const void *ptr)
{
	char fname[128];
	snprintf(fname, 128, MODEM_FOTA_SETTINGS_PREFIX "/%s", name);
	settings_save_one(fname, ptr, len);
}

void sb_fota_apply_update(void)
{
	FOTA_LOG_INF("Applying modem firmware update...");
	FOTA_LOG_DBG("Shutting down modem");

	if (fota_download_util_apply_update(DFU_TARGET_IMAGE_TYPE_MODEM_DELTA) == 0) {
		FOTA_LOG_DBG("Modem update OK");
	} else {
		FOTA_LOG_ERR("Modem update failed");
	}

	int err = lte_lc_connect();

	if (err) {
		FOTA_LOG_ERR("Connecting to network failed, err %d\n", err);
	}
}

NRF_MODEM_LIB_ON_INIT(sb_fota_init_hook, sb_fota_on_modem_init, NULL);

static void sb_fota_on_modem_init(int ret, void *ctx)
{
	int err;

	if (IS_ENABLED(CONFIG_SB_FOTA_AUTOINIT)) {
		err = sb_fota_init(NULL);
		if (err) {
			FOTA_LOG_ERR("Failed to initialize FOTA client");
		}
	}
}
