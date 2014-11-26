dnl $Id$
dnl config.m4 for extension cmark

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(cmark, for cmark support,
Make sure that the comment is aligned:
[  --with-cmark             Include cmark support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(cmark, whether to enable cmark support,
dnl Make sure that the comment is aligned:
dnl [  --enable-cmark           Enable cmark support])

if test "$PHP_CMARK" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-cmark -> check with-path
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="/include/cmark.h"  # you most likely want to change this
  if test -r $PHP_CMARK/$SEARCH_FOR; then # path given as parameter
    CMARK_DIR=$PHP_CMARK
  else # search default path list
    AC_MSG_CHECKING([for cmark files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        CMARK_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  dnl
  if test -z "$CMARK_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the cmark distribution])
  fi

  dnl # --with-cmark -> add include path
  PHP_ADD_INCLUDE($CMARK_DIR/include)

  dnl # --with-cmark -> check for lib and symbol presence
  LIBNAME=cmark # you may want to change this
  LIBSYMBOL=cmark_markdown_to_html # you most likely want to change this 

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $CMARK_DIR/$PHP_LIBDIR, CMARK_SHARED_LIBADD)
    AC_DEFINE(HAVE_CMARKLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong cmark lib version or lib not found])
  ],[
    -L$CMARK_DIR/$PHP_LIBDIR -lm
  ])
  dnl
  PHP_SUBST(CMARK_SHARED_LIBADD)

  PHP_NEW_EXTENSION(cmark, cmark.c, $ext_shared)
fi
