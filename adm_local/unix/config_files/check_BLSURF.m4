# Copyright (C) 2006-2008 OPEN CASCADE, CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# ---
#
# File   : check_BLSURF.m4
# Author : Vadim SANDLER, Open CASCADE S.A.S (vadim.sandler@opencascade.com)
#
# ---

AC_DEFUN([CHECK_BLSURF],[

AC_REQUIRE([AC_PROG_CXX])dnl
AC_REQUIRE([AC_PROG_CXXCPP])dnl

AC_CHECKING(for BLSURF commercial product)

AC_LANG_SAVE
AC_LANG_CPLUSPLUS

BLSURF_INCLUDES=""
BLSURF_LIBS=""

AC_ARG_WITH(blsurf,
	    [  --with-blsurf=DIR       root directory path of BLSURF installation])

BLSURF_ok=no

if test "$with_blsurf" == "no" ; then
    AC_MSG_FAILURE(Cannot build without BLSURF)
elif test "$with_blsurf" == "yes" || test "$with_blsurf" == "auto"; then
    BLSURF_HOME=""
else
    BLSURF_HOME="$with_blsurf"
fi

if test "$BLSURF_HOME" == "" ; then
    if test "x$BLSURFHOME" != "x" ; then
        BLSURF_HOME=$BLSURFHOME
    else
        AC_MSG_FAILURE(Cannot build without BLSURF. Use --with-blsurf option to define BLSURF installation.)
    fi
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
  CPPFLAGS="$BLSURF_INCLUDES $CPPFLAGS"
  CXXFLAGS="$BLSURF_INCLUDES $CXXFLAGS"

  AC_MSG_CHECKING(for BLSURF header file)

  AC_CHECK_HEADER(distene/api.h,BLSURF_ok=yes,BLSURF_ok=no)

  if test "x$BLSURF_ok" == "xyes"; then

    AC_MSG_CHECKING(for BLSURF library)

    LDFLAGS_old="$LDFLAGS"
    LDFLAGS="-L. -$BLSURF_LIBS $LDFLAGS"

    AC_TRY_LINK(
	#include "distene/api.h",
	BLSURF_init();,
	BLSURF_ok=yes,BLSURF_ok=no
	)

    LDFLAGS="$LDFLAGS_old"

    AC_MSG_RESULT($BLSURF_ok)
  fi

  CPPFLAGS="$CPPFLAGS_old"
  CXXFLAGS="$CXXFLAGS_old"

fi

if test "x$BLSURF_ok" == xno ; then
  AC_MSG_RESULT(for BLSURF: no)
  AC_MSG_WARN(BLSURF includes or libraries are not found or are not properly installed)
  AC_MSG_FAILURE(Cannot build without BLSURF. Use --with-blsurf option to define BLSURF installation.)
else
  AC_MSG_RESULT(for BLSURF: yes)
fi

AC_SUBST(BLSURF_INCLUDES)
AC_SUBST(BLSURF_LIBS)

AC_LANG_RESTORE

])dnl
