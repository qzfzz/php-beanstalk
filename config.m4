dnl $Id$
dnl config.m4 for extension beanstalk

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(beanstalk, for beanstalk support,
dnl Make sure that the comment is aligned:
dnl [  --with-beanstalk             Include beanstalk support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(beanstalk, whether to enable beanstalk support,
Make sure that the comment is aligned:
[  --enable-beanstalk           Enable beanstalk support])

if test "$PHP_BEANSTALK" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-beanstalk -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/beanstalk.h"  # you most likely want to change this
  dnl if test -r $PHP_BEANSTALK/$SEARCH_FOR; then # path given as parameter
  dnl   BEANSTALK_DIR=$PHP_BEANSTALK
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for beanstalk files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       BEANSTALK_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$BEANSTALK_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the beanstalk distribution])
  dnl fi

  dnl # --with-beanstalk -> add include path
  dnl PHP_ADD_INCLUDE($BEANSTALK_DIR/include)

  dnl # --with-beanstalk -> check for lib and symbol presence
  dnl LIBNAME=beanstalk # you may want to change this
  dnl LIBSYMBOL=beanstalk # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $BEANSTALK_DIR/$PHP_LIBDIR, BEANSTALK_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_BEANSTALKLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong beanstalk lib version or lib not found])
  dnl ],[
  dnl   -L$BEANSTALK_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(BEANSTALK_SHARED_LIBADD)

  PHP_NEW_EXTENSION(beanstalk, beanstalk.c, $ext_shared)
fi
