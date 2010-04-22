dnl  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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

dnl  File   : check_BLSURFPLUGIN.m4
dnl  Author : Vadim SANDLER, Open CASCADE S.A.S (vadim.sandler@opencascade.com)
dnl
AC_DEFUN([CHECK_BLSURFPLUGIN],[

AC_CHECKING(for BLSURF mesh plugin)

BLSURFplugin_ok=no

BLSURFPLUGIN_LDFLAGS=""
BLSURFPLUGIN_CXXFLAGS=""

AC_ARG_WITH(BLSURFplugin,
	    [  --with-BLSURFplugin=DIR root directory path of BLSURF mesh plugin installation ])

if test "$with_BLSURFplugin" != "no" ; then
    if test "$with_BLSURFplugin" == "yes" || test "$with_BLSURFplugin" == "auto"; then
	if test "x$BLSURFPLUGIN_ROOT_DIR" != "x" ; then
            BLSURFPLUGIN_DIR=$BLSURFPLUGIN_ROOT_DIR
        fi
    else
        BLSURFPLUGIN_DIR="$with_BLSURFplugin"
    fi

    if test "x$BLSURFPLUGIN_DIR" != "x" ; then
	if test -f ${BLSURFPLUGIN_DIR}/lib${LIB_LOCATION_SUFFIX}/salome/libBLSURFEngine.so ; then
	    BLSURFplugin_ok=yes
	    AC_MSG_RESULT(Using BLSURF mesh plugin distribution in ${BLSURFPLUGIN_DIR})
	    BLSURFPLUGIN_ROOT_DIR=${BLSURFPLUGIN_DIR}
	    BLSURFPLUGIN_LDFLAGS=-L${BLSURFPLUGIN_DIR}/lib${LIB_LOCATION_SUFFIX}/salome
	    BLSURFPLUGIN_CXXFLAGS=-I${BLSURFPLUGIN_DIR}/include/salome
	else
	    AC_MSG_WARN("Cannot find compiled BLSURF mesh plugin distribution")
	fi
    else
	AC_MSG_WARN("Cannot find compiled BLSURF mesh plugin distribution")
    fi
fi

AC_MSG_RESULT(for BLSURF mesh plugin: $BLSURFplugin_ok)

AC_SUBST(BLSURFPLUGIN_ROOT_DIR)
AC_SUBST(BLSURFPLUGIN_LDFLAGS)
AC_SUBST(BLSURFPLUGIN_CXXFLAGS)
 
])dnl
