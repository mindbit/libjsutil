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
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>

#include "config.h"
#include "jsmisc.h"

static const char * const log_prio_map[] = {
	[JS_LOG_EMERG]		= "emergency",
	[JS_LOG_ALERT]		= "alert",
	[JS_LOG_CRIT]		= "critical",
	[JS_LOG_ERR]		= "error",
	[JS_LOG_WARNING]	= "warning",
	[JS_LOG_NOTICE]		= "notice",
	[JS_LOG_INFO]		= "info",
	[JS_LOG_DEBUG]		= "debug"
};

struct str {
	char *buf;
	size_t strlen;
	size_t bufsize;
};

#define STR_CHUNK 4096
#define STR_INIT {NULL, 0, 0}

static int str_expand(struct str *str, size_t len)
{
	char *newbuf;
	size_t newsize;

	if (str->strlen + len <= str->bufsize)
		return 0;

	newsize = ((str->strlen + len + STR_CHUNK - 1) / STR_CHUNK) *
		STR_CHUNK;
	newbuf = realloc(str->buf, newsize);
	if (!newbuf)
		return -ENOMEM;

	str->buf = newbuf;
	str->bufsize = newsize;

	return 0;
}

static int str_printf(struct str *str, const char *format, ...)
{
	char buf[1024];
	va_list ap;
	int len, ret;

	va_start(ap, format);
	len = vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);

	if (len < 0 || len >= sizeof(buf))
		return -EINVAL;

	if ((ret = str_expand(str, len + 1)))
		return ret;

	memcpy(str->buf + str->strlen, buf, len);
	str->strlen += len;
	str->buf[str->strlen] = '\0';

	return 0;
}

static int str_append_js_str(struct str *str, JSContext *cx, JSString *js_str)
{
	int ret;
	size_t len = JS_GetStringEncodingLength(cx, js_str);

	if (len < 0)
		return -EINVAL;

	if ((ret = str_expand(str, len + 1)))
		return ret;

	len = JS_EncodeStringToBuffer(js_str, str->buf + str->strlen,
			str->bufsize - str->strlen);
	if (len < 0)
		return -EINVAL;

	str->strlen += len;
	str->buf[str->strlen] = '\0';
	return 0;
}

static void JS_LogDefaultCallback(int priority, const char *format,
		va_list ap)
{
	const char *prio_txt = "<default>";

	if (priority >= JS_LOG_EMERG && priority <= JS_LOG_DEBUG)
		prio_txt = log_prio_map[priority];

	fprintf(stderr, "[%s] ", prio_txt);
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

char *JS_StringToCStr(JSContext *cx, jsval v)
{
	JSString *value = JS_ValueToString(cx, v);

	if (value == NULL) {
		JS_Log(JS_LOG_ERR, "Failed to convert the value to JSString\n");
		goto out;
	}

	if (JS_AddStringRoot(cx, &value) == JS_FALSE) {
		JS_Log(JS_LOG_ERR, "Failed to root the string value\n");
		goto out;
	}

	char *value_str = JS_EncodeString(cx, value);
	if (value_str == NULL) {
		JS_Log(JS_LOG_ERR, "Failed to encode the string value\n");
		goto out_clean;
	}

	//FIXME not sure is this is safe
	JS_RemoveStringRoot(cx, &value);

	return value_str;

out_clean:
	JS_RemoveStringRoot(cx, &value);
out:
	return NULL;
}

static void JS_MiscErrorReporter(JSContext *cx, const char *message,
		JSErrorReport *report)
{
	int priority = (report->flags & JSREPORT_WARNING) ?
		JS_LOG_WARNING : JS_LOG_ERR;
	const char *filename = report->filename ?
		report->filename : "noname";

	JS_LogImpl(priority, "[%s:%u:%u] %s\n",
			filename,
			report->lineno,
			report->column,
			message);
}

JSErrorReporter JS_MiscSetErrorReporter(JSContext *cx)
{
	return JS_SetErrorReporter(cx, JS_MiscErrorReporter);
}

static JSBool JS_print(JSContext *cx, unsigned argc, jsval *vp)
{
	unsigned i;

	for (i = 0; i < argc; i++) {
		JSString *str = JS_ValueToString(cx, JS_ARGV(cx, vp)[i]);
		// FIXME check return value
		// FIXME root str (protect from GC) -> https://developer.mozilla.org/en-US/docs/SpiderMonkey/JSAPI_Reference/JS_ValueToString

		char *c_str = JS_EncodeString(cx, str);
		fputs(c_str, stdout);
		JS_free(cx, c_str);
	}

	return JS_TRUE;
}

static JSBool JS_println(JSContext *cx, unsigned argc, jsval *vp)
{
	JS_print(cx, argc, vp);
	putc('\n', stdout);
	return JS_TRUE;
}

static JSBool JS_gettimeofday(JSContext *cx, unsigned argc, jsval *vp)
{
	struct timeval tv;
	double us;
	jsval rval;

	gettimeofday(&tv, NULL);
	us =(double)tv.tv_sec * 1000000.0 + tv.tv_usec;
	rval = JS_NumberValue(us);

	JS_SET_RVAL(cx, vp, rval);
	return JS_TRUE;
}

static int JS_InspectRecursive(JSContext *cx, jsval v, struct str *str,
		unsigned int indent, unsigned int *obj_id)
{
	JSString *js_str;
	int ret = 0;
	size_t len;

	switch (JS_TypeOfValue(cx, v)) {
	case JSTYPE_VOID:
		ret = str_printf(str, "void");
		break;
	case JSTYPE_OBJECT:
		if (JSVAL_IS_NULL(v)) {
			ret = str_printf(str, "null");
			break;
		}
		if (JS_IsArrayObject(cx, JSVAL_TO_OBJECT(v))) {
			ret = str_printf(str, "Array");
			// TODO include array length
			// TODO iterate and recurse on array elements
			break;
		}
		if ((ret = str_printf(str, "Object {")))
			break;
		// TODO iterate and recurse on object properties
		ret = str_printf(str, "\n}");
		break;
	case JSTYPE_FUNCTION:
		ret = str_printf(str, "function");
		break;
	case JSTYPE_STRING:
		len = JS_GetStringLength(JSVAL_TO_STRING(v));
		if ((ret = str_printf(str, "String(%zu) \"", len)))
			break;
		if ((ret = str_append_js_str(str, cx, JSVAL_TO_STRING(v))))
			break;
		ret = str_printf(str, "\"");
		break;
	case JSTYPE_NUMBER:
		js_str = JS_ValueToString(cx, v);
		if ((ret = str_printf(str, "Number(")))
			break;
		if ((ret = str_append_js_str(str, cx, js_str)))
			break;
		ret = str_printf(str, ")");
		break;
	case JSTYPE_BOOLEAN:
		js_str = JS_ValueToString(cx, v);
		if ((ret = str_printf(str, "Boolean(")))
			break;
		if ((ret = str_append_js_str(str, cx, js_str)))
			break;
		ret = str_printf(str, ")");
		break;
	default:
		ret = str_printf(str, "FIXME");
		break;
	}

	return ret;
}

static int JS_InspectRoot(JSContext *cx, unsigned argc, jsval *vp,
		struct str *str)
{
	int ret = 0;
	unsigned i;

	for (i = 0; i < argc; i++) {
		unsigned int obj_id = 0;
		if ((ret = str_printf(str, "$%u = ", i)))
			break;
		if (JS_InspectRecursive(cx, JS_ARGV(cx, vp)[i], str, 1, &obj_id))
			break;
		if ((ret = str_printf(str, "\n")))
			break;
	}

	return ret;
}

static JSBool JS_inspect(JSContext *cx, unsigned argc, jsval *vp)
{
	JSString *js_str;
	struct str c_str = STR_INIT;
	JSBool ret = JS_FALSE;

	if (!argc) {
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
		return JS_TRUE;
	}

	if (JS_InspectRoot(cx, argc, vp, &c_str))
		goto out;

	js_str = JS_NewStringCopyN(cx, c_str.buf, c_str.strlen);
	if (!js_str)
		goto out;

	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(js_str));
	ret = JS_TRUE;

out:
	free(c_str.buf);
	return ret;
}

static JSBool JS_dump(JSContext *cx, unsigned argc, jsval *vp)
{
	struct str c_str = STR_INIT;

	if (!argc)
		return JS_TRUE;

	if (!JS_InspectRoot(cx, argc, vp, &c_str))
		fputs(c_str.buf, stdout);

	free(c_str.buf);
	return JS_TRUE;
}

JSBool JS_MiscInit(JSContext *cx, JSObject *global)
{
	JS_Log(JS_LOG_INFO, "%s\n", VERSION_STR);
	JS_DefineFunction(cx, global, "print", JS_print, 0, 0);
	JS_DefineFunction(cx, global, "println", JS_println, 0, 0);
	JS_DefineFunction(cx, global, "gettimeofday", JS_gettimeofday, 0, 0);
	JS_DefineFunction(cx, global, "inspect", JS_inspect, 0, 0);
	JS_DefineFunction(cx, global, "dump", JS_dump, 0, 0);
	return JS_TRUE;
}
