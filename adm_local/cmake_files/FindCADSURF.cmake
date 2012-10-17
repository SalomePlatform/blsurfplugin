# Copyright (C) 2007-2012  CEA/DEN, EDF R&D
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

SET(MESHGEMSHOME $ENV{MESHGEMSHOME})
FIND_PATH(CADSURF_INCLUDES_DIR meshgems/cadsurf.h ${MESHGEMSHOME}/include)
SET(CADSURF_INCLUDES)
SET(CADSURF_INCLUDES ${CADSURF_INCLUDES} -I${CADSURF_INCLUDES_DIR})

SET(CADSURF_LIBS_PATHS)
SET(CADSURF_LIBS_PATHS ${CADSURF_LIBS_PATHS} ${MESHGEMSHOME}/lib)
IF(WINDOWS)
  SET(CADSURF_LIBS_PATHS ${CADSURF_LIBS_PATHS} ${MESHGEMSHOME}/lib/WinXP_VC9)
ELSE(WINDOWS)
  IF(CMAKE_SIZEOF_VOID_P STREQUAL 8)
    SET(CADSURF_LIBS_PATHS ${CADSURF_LIBS_PATHS} ${MESHGEMSHOME}/lib/Linux_64)
  ELSE()
    SET(CADSURF_LIBS_PATHS ${CADSURF_LIBS_PATHS} ${MESHGEMSHOME}/lib/Linux)
  ENDIF()
ENDIF(WINDOWS)

FIND_LIBRARY(mg-cadsurf mg-cadsurf PATHS ${CADSURF_LIBS_PATHS})
FIND_LIBRARY(mg-precad mg-precad PATHS ${CADSURF_LIBS_PATHS})
FIND_LIBRARY(meshgems meshgems PATHS ${CADSURF_LIBS_PATHS})

SET(CADSURF_LIBS)
SET(CADSURF_LIBS ${CADSURF_LIBS} ${mg-cadsurf})
SET(CADSURF_LIBS ${CADSURF_LIBS} ${mg-precad})
IF(meshgems)
  SET(CADSURF_LIBS ${CADSURF_LIBS} ${meshgems})
ENDIF(meshgems)
