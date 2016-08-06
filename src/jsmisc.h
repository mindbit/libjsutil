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

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*js_log_callback_t)(int priority, const char *format,
		va_list ap);

#ifdef JSMISC_DEBUG
#define js_log(priority, format, ...) do {				\
	__js_log(priority, "[%s %s:%d] " format, __func__, __FILE__,	\
			__LINE__, ##__VA_ARGS__)			\
} while (0)
#else
#define js_log(priority, format, ...) do {				\
	__js_log(priority, "[%s] " format, __func__, ##__VA_ARGS__)	\
} while (0)
#endif

void __js_log(int priority, const char *format, va_list ap);

void js_log_set_callback(js_log_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif
