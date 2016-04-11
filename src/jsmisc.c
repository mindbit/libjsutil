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
