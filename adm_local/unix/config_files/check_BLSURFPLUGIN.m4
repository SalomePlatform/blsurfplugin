AC_DEFUN([CHECK_BLSURF],[

AC_REQUIRE([AC_PROG_CXX])dnl
AC_REQUIRE([AC_PROG_CXXCPP])dnl

AC_CHECKING(for BLSURF comercial product)

AC_LANG_SAVE
AC_LANG_CPLUSPLUS

AC_ARG_WITH(,
	    [  --with-BLSURF=DIR root directory path of BLSUF installation],
	    BLSURF_HOME=$withval,BLSURF_HOME="")

BLSURF_INCLUDES=""
BLSURF_LIBS=""

BLSURF_ok=no

if test "x$BLSURF_HOME" == "x" ; then

# no --with-BLSURF option used
   if test "x$BLSURFHOME" != "x" ; then

    # BLSURFHOME environment variable defined
      BLSURF_HOME=$BLSURFHOME

   fi
# 
fi

if test "x$BLSURF_HOME" != "x"; then

  echo
  echo -------------------------------------------------
  echo You are about to choose to use somehow the
  echo BLSURF commercial product to generate 2D mesh.
  echo

  

  BLSURF_INCLUDES="-I$BLSURF_HOME/include"
  BLSURF_LIBS="-L$BLSURF_HOME/lib -lBLSurf"

  CPPFLAGS_old="$CPPFLAGS"
  CXXFLAGS_old="$CXXFLAGS"
  LDFLAGS_old="$LDFLAGS"

  CPPFLAGS="$BLSURF_INCLUDES $CPPFLAGS"
  CXXFLAGS="$BLSURF_INCLUDES $CXXFLAGS"
  LDFLAGS="-L. -$BLSURF_LIBS $LDFLAGS"

  AC_MSG_CHECKING(for BLSURF header file)

  AC_CHECK_HEADER(distene/api.h,BLSURF_ok=yes,BLSURF_ok=no)
  if test "x$BLSURF_ok" == "xyes"; then

    AC_MSG_CHECKING(for BLSURF library)

    AC_TRY_COMPILE(#include "distene/api.h",
        BLSURF_init();
     ,BLSURF_ok=yes;
      $CXX -shared -o linopt.o -c
     ,BLSURF_ok=no)

  fi

  CPPFLAGS="$CPPFLAGS_old"
  CXXFLAGS="$CXXFLAGS_old"
  LDFLAGS="$LDFLAGS_old"

  if test "x$BLSURF_ok" == xno ; then
    AC_MSG_RESULT(no)
    AC_MSG_WARN(BLSURF includes or libraries are not found or are not properly installed)
  else
    AC_MSG_RESULT(yes)
  fi

fi

AC_SUBST(BLSURF_INCLUDES)
AC_SUBST(BLSURF_LIBS)

AC_LANG_RESTORE

])dnl
