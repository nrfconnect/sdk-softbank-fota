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
#include <zephyr/sys/timeutil.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_msg.h>
#include <zephyr/settings/settings.h>
#include <modem/nrf_modem_lib.h>
#include <modem/lte_lc.h>
#include <fota_download_util.h>

/* Memory management. */

void *sb_fota_os_malloc(size_t size)
{
	return k_malloc(size);
}

void *sb_fota_os_calloc(size_t nmemb, size_t size)
{
	return k_calloc(nmemb, size);
}

void sb_fota_os_free(void *ptr)
{
	k_free(ptr);
}

/* Timing functions. */

int64_t sb_fota_os_uptime_get(void)
{
	return k_uptime_get();
}

uint32_t sb_fota_os_uptime_get_32(void)
{
	return k_uptime_get_32();
}

int sb_fota_os_sleep(int ms)
{
	return k_sleep(K_MSEC(ms));
}

/* OS functions */

uint32_t sb_fota_os_rand_get(void)
{
	return sys_rand32_get();
}

/* Semaphores */

struct sb_fota_os_sem {
	struct k_sem sem;
};

static struct sb_fota_os_sem semaphores[SB_FOTA_OS_SEMAPHORE_COUNT];

struct sb_fota_os_sem *sb_fota_os_sem_alloc(void) {
	static int next_free;

	if (next_free >= SB_FOTA_OS_SEMAPHORE_COUNT) {
		return NULL;
	}

	struct sb_fota_os_sem *sem = &semaphores[next_free++];
	k_sem_init(&sem->sem, 0, 1);
	return sem;
}

void sb_fota_os_sem_give(struct sb_fota_os_sem *sem)
{
	k_sem_give(&sem->sem);
}

int sb_fota_os_sem_take(struct sb_fota_os_sem *sem, int timeout_ms)
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

void sb_fota_os_sem_reset(struct sb_fota_os_sem *sem)
{
	k_sem_reset(&sem->sem);
}

/* Work queue */
#define MY_PRIORITY 5

K_THREAD_STACK_DEFINE(sb_fota_stack_area, CONFIG_MAIN_STACK_SIZE);

static struct k_work_q sb_fota_work_q;
static bool q_is_initialized = false;

struct sb_fota_os_work {
	struct k_work w;
};
struct sb_fota_os_delayed_work {
	struct k_work_delayable dw;
};

static struct sb_fota_os_work works[SB_FOTA_OS_WORK_COUNT];
static struct sb_fota_os_delayed_work delayed_works[SB_FOTA_OS_DELAYED_WORK_COUNT];

struct sb_fota_os_work *sb_fota_os_work_init(void (*callback)(void *))
{
	static int next_free;

	if (!q_is_initialized) {
		k_work_queue_init(&sb_fota_work_q);
		k_work_queue_start(&sb_fota_work_q, sb_fota_stack_area,
				   K_THREAD_STACK_SIZEOF(sb_fota_stack_area), MY_PRIORITY, NULL);
		k_thread_name_set(&sb_fota_work_q.thread, "sb_fota");
		q_is_initialized = true;
	}

	if (next_free >= SB_FOTA_OS_WORK_COUNT) {
		return NULL;
	}

	struct sb_fota_os_work *work = &works[next_free++];
	k_work_init(&work->w, (void (*)(struct k_work *)) callback);
	return work;
}

void sb_fota_os_work_schedule(struct sb_fota_os_work *work)
{
	k_work_submit_to_queue(&sb_fota_work_q, & work->w);
}

struct sb_fota_os_delayed_work *sb_fota_os_delayed_work_init(void (*callback)(void *))
{
	static int next_free;

	if (next_free >= SB_FOTA_OS_DELAYED_WORK_COUNT) {
		return NULL;
	}

	struct sb_fota_os_delayed_work *work = &delayed_works[next_free++];
	k_work_init_delayable(&work->dw, (void (*)(struct k_work *)) callback);
	return work;
}

void sb_fota_os_delayed_work_schedule(struct sb_fota_os_delayed_work *work, int delay_ms)
{
	k_work_schedule_for_queue(&sb_fota_work_q, &work->dw, K_MSEC(delay_ms));
}

/* Timers */

struct sb_fota_os_timer {
	struct k_timer t;
};

struct sb_fota_os_timer timers[SB_FOTA_OS_TIMERS];

struct sb_fota_os_timer *sb_fota_os_timer_init(void (*callback)(void *))
{
	static int next_free;

	if (next_free >= SB_FOTA_OS_TIMERS) {
		return NULL;
	}

	struct sb_fota_os_timer *timer = &timers[next_free++];
	k_timer_init(&timer->t, (void (*)(struct k_timer *))callback, NULL);
	return timer;
}

void sb_fota_os_timer_start(struct sb_fota_os_timer *timer, uint64_t delay_ms)
{
	k_timeout_t timeout = (delay_ms ? K_MSEC(delay_ms) : K_NO_WAIT);
	k_timer_start(&timer->t, timeout, K_NO_WAIT);
}

void sb_fota_os_timer_stop(struct sb_fota_os_timer *timer)
{
	k_timer_stop(&timer->t);
}

bool sb_fota_os_timer_is_running(struct sb_fota_os_timer *timer)
{
	return k_timer_remaining_get(&timer->t) != 0;
}

int64_t sb_fota_os_timegm64(const struct tm *time)
{
	return timeutil_timegm64(time);
}

/* Logging */
LOG_MODULE_REGISTER(sb_fota, CONFIG_SB_FOTA_LOG_LEVEL);

#if defined(CONFIG_LOG)
static uint8_t log_level_translate(uint8_t level)
{
	switch (level) {
	case SB_FOTA_OS_LOG_LEVEL_ERR:
		return LOG_LEVEL_ERR;
	case SB_FOTA_OS_LOG_LEVEL_WRN:
		return LOG_LEVEL_WRN;
	case SB_FOTA_OS_LOG_LEVEL_INF:
		return LOG_LEVEL_INF;
	case SB_FOTA_OS_LOG_LEVEL_DBG:
		return LOG_LEVEL_DBG;
	default:
		return LOG_LEVEL_NONE;
	}
}
#endif

void sb_fota_os_log(int level, const char *fmt, ...)
{
#if defined(CONFIG_LOG)
	level = log_level_translate(level);
	if (level > CONFIG_SB_FOTA_LOG_LEVEL) {
		return;
	}

	va_list ap;
	va_start(ap, fmt);

#if CONFIG_LOG_MODE_MINIMAL
		/* Fallback to minimal implementation. */
		printk("%c: ", z_log_minimal_level_to_char(level));
		z_log_minimal_vprintk(fmt, ap);
		printk("\n");
#else
		void *source;

		if (IS_ENABLED(CONFIG_LOG_RUNTIME_FILTERING)) {
			source = (void *)__log_current_dynamic_data;
		} else {
			source = (void *)__log_current_const_data;
		}

		z_log_msg_runtime_vcreate(Z_LOG_LOCAL_DOMAIN_ID, source, level,
					  NULL, 0, 0, fmt, ap);
#endif /* CONFIG_LOG_MODE_MINIMAL */

	va_end(ap);
#endif /* CONFIG_LOG */
}

/* Settings */

static const struct sb_fota_settings *sb_fota_settings;

static int settings_set(const char *name, size_t len,
		settings_read_cb read_cb, void *cb_arg)
{
	void *data;

	for (const struct sb_fota_settings *sp = sb_fota_settings; sp->name; ++sp) {
		if (strcmp(name, sp->name)) {
			continue;
		}
		/* Do I need to allocate the storage? */
		if (sp->len > 0) {
			data = sp->ptr;
		} else {
			data = sb_fota_os_malloc(len);
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
				sb_fota_os_free(data);
				*((void**)sp->ptr) = NULL;
			}
			return -EINVAL;
		}
	}
	return -ENOENT;
}

void sb_fota_os_load_settings(const struct sb_fota_settings *settings)
{
	sb_fota_settings = settings;

	settings_subsys_init();

	struct settings_handler my_conf = {
	    .name = SB_FOTA_SETTINGS_PREFIX,
	    .h_set = settings_set
	};

	settings_register(&my_conf);
	settings_load_subtree(SB_FOTA_SETTINGS_PREFIX);
}


void sb_fota_os_store_setting(const char *name, size_t len, const void *ptr)
{
	char fname[128];
	snprintf(fname, 128, SB_FOTA_SETTINGS_PREFIX "/%s", name);
	settings_save_one(fname, ptr, len);
}

void sb_fota_os_update_apply(void)
{
	LOG_INF("Applying modem firmware update...");
	LOG_DBG("Shutting down modem");

	if (fota_download_util_apply_update(DFU_TARGET_IMAGE_TYPE_MODEM_DELTA) == 0) {
		LOG_DBG("Modem update OK");
	} else {
		LOG_ERR("Modem update failed");
	}

	int err = lte_lc_connect();

	if (err) {
		LOG_ERR("Connecting to network failed, err %d", err);
	}
}

NRF_MODEM_LIB_ON_INIT(sb_fota_init_hook, sb_fota_on_modem_init, NULL);

static void sb_fota_on_modem_init(int ret, void *ctx)
{
	int err;

	if (IS_ENABLED(CONFIG_SB_FOTA_AUTOINIT)) {
		err = sb_fota_init(NULL);
		if (err) {
			LOG_ERR("Failed to initialize FOTA client");
		}
	}
}
