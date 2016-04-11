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
