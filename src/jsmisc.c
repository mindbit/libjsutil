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

#include <stdio.h>
#include <stdarg.h>

#include "jsmisc.h"

static void JS_LogDefaultCallback(int priority, const char *format,
		va_list ap)
{
	fprintf(stderr, "[%d] ", priority);
	vfprintf(stderr, format, ap);
}

static JSLogCallback js_log_callback = JS_LogDefaultCallback;

void JS_LogImpl(int priority, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	if (js_log_callback)
		js_log_callback(priority, format, ap);
	va_end(ap);
}

void JS_LogSetCallback(JSLogCallback callback)
{
	js_log_callback = callback;
}
