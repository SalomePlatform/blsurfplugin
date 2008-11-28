dnl  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
dnl
dnl  This library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public
dnl  License as published by the Free Software Foundation; either
dnl  version 2.1 of the License.
dnl
dnl  This library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with this library; if not, write to the Free Software
dnl  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl
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
