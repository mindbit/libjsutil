AC_PREREQ(2.61)
# Checks for programs.
AC_INIT([libjsmisc], [0.0.1], [support@mindbit.ro])

AC_CANONICAL_TARGET
AC_CANONICAL_HOST
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([jsmisc_config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_PROG_CC_C_O
LT_INIT([disable-shared])
AC_SUBST(MKDIR_P)


CFLAGS="-Wall $CFLAGS"


# additional configure arguments

AC_ARG_WITH(js-include,
    AC_HELP_STRING([--with-js-include=DIR], [search for mozjs (spidermonkey) headers in DIR]),
    [
        JS_INCLUDE="$withval"
    ]
)

AC_ARG_WITH(js-lib,
    AC_HELP_STRING([--with-js-lib=NAME], [specify mozjs (spidermonkey) library name]),
    [
        JS_LIB="$withval"
    ]
)


# test for SpiderMonkey 17.0 (mozjs 17.0)

if test -z "$JS_INCLUDE" ; then
    JS_INCLUDE=" \
        /usr/include/js-17.0 \
        /usr/local/include/js-17.0 \
    "
    AC_CHECK_HEADER(jsapi.h,[],[],-)
fi
for incpath in $JS_INCLUDE; do
    if test "x$ac_cv_header_jsapi_h" != xyes ; then
        AC_MSG_CHECKING([for jsapi.h in $incpath])
        save_cppflags="$CPPFLAGS"
        CPPFLAGS="-I$incpath $CPPFLAGS"
        AC_PREPROC_IFELSE([AC_LANG_PROGRAM([[
                #include <jsapi.h>
            ]])],
            [
                AC_MSG_RESULT(yes)
                ac_cv_header_jsapi_h=yes
            ],
            [
                AC_MSG_RESULT(no)
                CPPFLAGS="$save_cppflags"
            ]
        )
   fi
done
if test "x$ac_cv_header_jsapi_h" != xyes ; then
    AC_MSG_ERROR([jsapi.h not found])
fi

if test -z "$JS_LIB" ; then
    JS_LIB=" \
        mozjs-17.0 \
        mozjs \
    "
fi
for lib in $JS_LIB; do
    if test "x$js_lib_found" != xyes ; then
        AC_CHECK_LIB([$lib],[JS_NewContext],[
            LIBS="-l$lib $LIBS"
            js_lib_found=yes
        ])
    fi
done
if test "x$js_lib_found" != xyes ; then
    AC_MSG_ERROR([no usable mozjs (spidermonkey) library found])
fi

AC_CONFIG_FILES([
Makefile
src/Makefile
])
AC_OUTPUT
