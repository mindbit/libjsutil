/* libjsmisc - Miscellaneous utility functions for SpiderMonkey JS
 * Copyright (C) 2016 Mindbit SRL
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of version 2.1 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _JSMISC_H
#define _JSMISC_H

#include <stdarg.h>
#include <duktape.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Logging priorities. These correspond 1:1 to the syslog priorities
 * (see /usr/include/sys/syslog.h), but are redefined to avoid an
 * explicit dependency against syslog.
 */
#define	JS_LOG_EMERG	0	/* system is unusable */
#define	JS_LOG_ALERT	1	/* action must be taken immediately */
#define	JS_LOG_CRIT	2	/* critical conditions */
#define	JS_LOG_ERR	3	/* error conditions */
#define	JS_LOG_WARNING	4	/* warning conditions */
#define	JS_LOG_NOTICE	5	/* normal but significant condition */
#define	JS_LOG_INFO	6	/* informational */
#define	JS_LOG_DEBUG	7	/* debug-level messages */

typedef void (*js_log_callback_t)(int priority, const char *format,
		va_list ap);

#ifdef JS_DEBUG
#define js_log(priority, format, ...)					\
	js_log_impl(priority, "[%s %s:%d] " format, __func__, __FILE__,	\
			__LINE__, ##__VA_ARGS__)
#else
#define js_log(priority, format, ...) do {				\
	if (priority < JS_LOG_DEBUG)					\
		js_log_impl(priority, "[%s] " format,			\
				__func__, ##__VA_ARGS__);		\
} while (0)
#endif

void js_log_impl(int priority, const char *format, ...);
void js_log_set_callback(js_log_callback_t callback);

/**
 * @brief Append an element at the end of an array
 */
duk_bool_t js_append_array_element(duk_context *ctx, duk_idx_t obj_idx);

duk_bool_t js_misc_init(duk_context *ctx, duk_idx_t obj_idx);

#ifdef __cplusplus
}
#endif

#endif
