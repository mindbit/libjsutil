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
#include <jsapi.h>

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

typedef void (*JSLogCallback)(int priority, const char *format,
		va_list ap);

#ifdef JS_DEBUG
#define JS_Log(priority, format, ...)					\
	JS_LogImpl(priority, "[%s %s:%d] " format, __func__, __FILE__,	\
			__LINE__, ##__VA_ARGS__)
#else
#define JS_Log(priority, format, ...) do {				\
	if (priority < JS_LOG_DEBUG)					\
		JS_LogImpl(priority, "[%s] " format,			\
				__func__, ##__VA_ARGS__);		\
} while (0)
#endif

void JS_LogImpl(int priority, const char *format, ...);
void JS_LogSetCallback(JSLogCallback callback);

/**
 * Convert JSString value to C string (array of chars)
 * @param cx	JavaScript context
 * @param v	JSString value
 *
 * @return A pointer to the converted value on success, or NULL on failure
 */
char *JS_StringToCStr(JSContext *cx, jsval v);

JSErrorReporter JS_MiscSetErrorReporter(JSContext *cx);

#ifdef __cplusplus
}
#endif

#endif
