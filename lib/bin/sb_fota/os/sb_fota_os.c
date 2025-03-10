/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "sb_fota_os.h"
#include "sb_fota.h"
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include <zephyr/sys/timeutil.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_msg.h>
#include <zephyr/settings/settings.h>
#include <modem/nrf_modem_lib.h>
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

int64_t sb_fota_os_timegm64(const struct tm *time)
{
	return timeutil_timegm64(time);
}

/* Random number generation. */

uint32_t sb_fota_os_rand_get(void)
{
	return sys_rand32_get();
}

/* Semaphores. */

#define SB_FOTA_SEMAPHORE_COUNT 4

static struct k_sem semaphores[SB_FOTA_SEMAPHORE_COUNT];

sb_fota_os_sem_t *sb_fota_os_sem_alloc(void) {
	static int next_free;

	if (next_free >= SB_FOTA_SEMAPHORE_COUNT) {
		return NULL;
	}

	sb_fota_os_sem_t *sem = (sb_fota_os_sem_t *)&semaphores[next_free++];
	k_sem_init((struct k_sem *)sem, 0, 1);
	return sem;
}

void sb_fota_os_sem_give(sb_fota_os_sem_t *sem)
{
	k_sem_give((struct k_sem *)sem);
}

int sb_fota_os_sem_take(sb_fota_os_sem_t *sem, int timeout_ms)
{
	k_timeout_t t;
	if (timeout_ms == 0) {
		t = K_NO_WAIT;
	} else if (timeout_ms < 0) {
		t = K_FOREVER;
	} else {
		t = K_MSEC(timeout_ms);
	}
	return k_sem_take((struct k_sem *)sem, t);
}

void sb_fota_os_sem_reset(sb_fota_os_sem_t *sem)
{
	k_sem_reset((struct k_sem *)sem);
}

/* Work queue. */

#define SB_FOTA_WORK_QUEUE_PRIORITY 5
#define SB_FOTA_WORK_COUNT 5
#define SB_FOTA_DELAYED_WORK_COUNT 1

K_THREAD_STACK_DEFINE(sb_fota_stack_area, CONFIG_MAIN_STACK_SIZE);

static struct k_work_q sb_fota_work_q;
static struct k_work works[SB_FOTA_WORK_COUNT];
static struct k_work_delayable delayed_works[SB_FOTA_DELAYED_WORK_COUNT];

sb_fota_os_work_t *sb_fota_os_work_init(void (*callback)(void *))
{
	static bool is_initialized;
	static int next_free;

	if (!is_initialized) {
		k_work_queue_init(&sb_fota_work_q);
		k_work_queue_start(&sb_fota_work_q, sb_fota_stack_area,
				   K_THREAD_STACK_SIZEOF(sb_fota_stack_area),
				   SB_FOTA_WORK_QUEUE_PRIORITY, NULL);
		k_thread_name_set(&sb_fota_work_q.thread, "sb_fota");
		is_initialized = true;
	}

	if (next_free >= SB_FOTA_WORK_COUNT) {
		return NULL;
	}

	sb_fota_os_work_t *work = (sb_fota_os_work_t *)&works[next_free++];
	k_work_init((struct k_work *)work, (void (*)(struct k_work *)) callback);
	return work;
}

void sb_fota_os_work_schedule(sb_fota_os_work_t *work)
{
	k_work_submit_to_queue(&sb_fota_work_q, (struct k_work *)work);
}

sb_fota_os_delayed_work_t *sb_fota_os_delayed_work_init(void (*callback)(void *))
{
	static int next_free;

	if (next_free >= SB_FOTA_DELAYED_WORK_COUNT) {
		return NULL;
	}

	sb_fota_os_delayed_work_t *work = (sb_fota_os_delayed_work_t *)&delayed_works[next_free++];
	k_work_init_delayable((struct k_work_delayable *)work, (void (*)(struct k_work *)) callback);
	return work;
}

void sb_fota_os_delayed_work_schedule(sb_fota_os_delayed_work_t *work, int delay_ms)
{
	k_work_schedule_for_queue(&sb_fota_work_q, (struct k_work_delayable *)work, K_MSEC(delay_ms));
}

/* Timers. */

#define SB_FOTA_TIMER_COUNT 2

struct k_timer timers[SB_FOTA_TIMER_COUNT];

sb_fota_os_timer_t *sb_fota_os_timer_init(void (*callback)(void *))
{
	static int next_free;

	if (next_free >= SB_FOTA_TIMER_COUNT) {
		return NULL;
	}

	sb_fota_os_timer_t *timer = (sb_fota_os_timer_t *)&timers[next_free++];
	k_timer_init((struct k_timer *)timer, (void (*)(struct k_timer *))callback, NULL);
	return timer;
}

void sb_fota_os_timer_start(sb_fota_os_timer_t *timer, uint64_t delay_ms)
{
	k_timeout_t timeout = (delay_ms ? K_MSEC(delay_ms) : K_NO_WAIT);
	k_timer_start((struct k_timer *)timer, timeout, K_NO_WAIT);
}

void sb_fota_os_timer_stop(sb_fota_os_timer_t *timer)
{
	k_timer_stop((struct k_timer *)timer);
}

bool sb_fota_os_timer_is_running(sb_fota_os_timer_t *timer)
{
	return k_timer_remaining_get((struct k_timer *)timer) != 0;
}

/* Logging. */

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

/* Settings. */

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

/* Network control. */

#if defined(CONFIG_LTE_LINK_CONTROL)
#include <modem/lte_lc.h>

static enum lte_lc_system_mode prev_system_mode;
static enum lte_lc_system_mode_preference prev_system_mode_pref;

int sb_fota_os_lte_online(void)
{
	return lte_lc_normal();
}

int sb_fota_os_lte_offline(void)
{
	return lte_lc_offline();
}

int sb_fota_os_lte_mode_request_ltem(void)
{
	int err;

	err = lte_lc_system_mode_get(&prev_system_mode, &prev_system_mode_pref);
	if (err) {
		LOG_ERR("Failed to read system mode, err %d", err);
		return err;
	}

	err = lte_lc_system_mode_set(LTE_LC_SYSTEM_MODE_LTEM, LTE_LC_SYSTEM_MODE_PREFER_AUTO);
	if (err) {
		LOG_ERR("Failed to set system mode, err %d", err);
	}

	return err;
}

int sb_fota_os_lte_mode_restore(void)
{
	return lte_lc_system_mode_set(prev_system_mode, prev_system_mode_pref);
}
#else
#include <nrf_modem_at.h>

static int prev_system_mode_ltem;
static int prev_system_mode_nbiot;
static int prev_system_mode_gps;
static int prev_system_mode_preference;

int sb_fota_os_lte_online(void)
{
	return nrf_modem_at_printf("AT+CFUN=1");
}

int sb_fota_os_lte_offline(void)
{
	return nrf_modem_at_printf("AT+CFUN=4");
}

int sb_fota_os_lte_mode_request_ltem(void)
{
	int err;

	err = nrf_modem_at_scanf("AT%XSYSTEMMODE?", "%%XSYSTEMMODE: %d,%d,%d,%d",
				 &prev_system_mode_ltem, &prev_system_mode_nbiot,
				 &prev_system_mode_gps, &prev_system_mode_preference);
	if (err != 4) {
		LOG_ERR("Failed to read system mode, err %d", err);
		return -EIO;
	}

	err = nrf_modem_at_printf("AT%%XSYSTEMMODE=1,0,0,0");
	if (err) {
		LOG_ERR("Failed to set system mode, err %d", err);
	}

	return err;
}

int sb_fota_os_lte_mode_restore(void)
{
	int err;

	err = nrf_modem_at_printf("AT%%XSYSTEMMODE=%d,%d,%d,%d",
				  prev_system_mode_ltem, prev_system_mode_nbiot,
				  prev_system_mode_gps, prev_system_mode_preference);
	if (err) {
		LOG_ERR("Failed to set system mode, err %d", err);
		return -EIO;
	}

	return 0;
}
#endif /* CONFIG_LTE_LINK_CONTROL */

/* Firmware update. */

void sb_fota_os_update_apply(void)
{
	LOG_INF("Applying modem firmware update...");
	LOG_DBG("Shutting down modem");

	if (fota_download_util_apply_update(DFU_TARGET_IMAGE_TYPE_MODEM_DELTA) == 0) {
		LOG_DBG("Modem update OK");
	} else {
		LOG_ERR("Modem update failed");
	}

	if (sb_fota_os_lte_online()) {
		LOG_ERR("Failed to bring the modem online");
	}
}

/* Modem library hooks. */

NRF_MODEM_LIB_ON_INIT(sb_fota_init_hook, on_modem_init, NULL);

static void on_modem_init(int ret, void *ctx)
{
	int err;

	if (IS_ENABLED(CONFIG_SB_FOTA_AUTOINIT)) {
		err = sb_fota_init(NULL);
		if (err) {
			LOG_ERR("Failed to initialize SoftBank FOTA library");
		}
	}
}

NRF_MODEM_LIB_ON_CFUN(sb_fota_cfun_hook, on_modem_cfun, NULL);

static void on_modem_cfun(int mode, void *ctx)
{
	ARG_UNUSED(ctx);

	int err = sb_fota_on_modem_cfun(mode ? SB_FOTA_MODEM_MODE_POWER_ON :
					       SB_FOTA_MODEM_MODE_POWER_OFF);
	if (err) {
		LOG_ERR("SoftBank FOTA CFUN handler failed, err %d", err);
	}
}
