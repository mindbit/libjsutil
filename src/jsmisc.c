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

static void js_log_default_callback(int priority, const char *format,
		va_list ap)
{
	fprintf(stderr, "[%d] ", priority);
	vfprintf(stderr, format, ap);
}

static js_log_callback_t js_log_callback = js_log_default_callback;

void __js_log(int priority, const char *format, va_list ap)
{
	if (js_log_callback)
		js_log_callback(priority, format, ap);
}

void js_log_set_callback(js_log_callback_t callback)
{
	js_log_callback = callback;
}
