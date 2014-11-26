PHP_ARG_WITH(cmark, for cmark support,
[  --with-cmark             Include cmark support])

if test "$PHP_CMARK" != "no"; then

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
  
  if test -z "$CMARK_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the cmark distribution])
  fi

  PHP_ADD_INCLUDE($CMARK_DIR/include)

  LIBNAME=cmark
  LIBSYMBOL=cmark_new_doc_parser

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $CMARK_DIR/$PHP_LIBDIR, CMARK_SHARED_LIBADD)
    AC_DEFINE(HAVE_CMARKLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong cmark lib version or lib not found])
  ],[
    -L$CMARK_DIR/$PHP_LIBDIR -lm
  ])
  
  PHP_SUBST(CMARK_SHARED_LIBADD)

  PHP_NEW_EXTENSION(cmark, cmark.c, $ext_shared)
fi
