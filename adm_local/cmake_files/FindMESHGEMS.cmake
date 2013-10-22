# - Find MESHGEMS
# Sets the following variables:
#   MESHGEMS_INCLUDE_DIRS - path to the MESHGEMS include directory
#   MESHGEMS_LIBRARIES    - path to the MESHGEMS libraries to be linked against
#

# Copyright (C) 2007-2013  CEA/DEN, EDF R&D
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

# ------

MESSAGE(STATUS "Check for MESHGEMS ...")

# ------

SET(MESHGEMS_ROOT_DIR $ENV{MESHGEMS_ROOT_DIR})

FIND_PATH(MESHGEMS_INCLUDE_DIRS meshgems/cadsurf.h ${MESHGEMS_ROOT_DIR}/include)

SET(CADSURF_LIBS_PATHS)
SET(CADSURF_LIBS_PATHS ${CADSURF_LIBS_PATHS} ${MESHGEMS_ROOT_DIR}/lib)

SET(suff)
IF(MACHINE_IS_64)
  SET(suff "_64")
ENDIF(MACHINE_IS_64)

IF(WIN32)
  SET(CADSURF_LIBS_PATHS ${CADSURF_LIBS_PATHS} ${MESHGEMS_ROOT_DIR}/lib/WinXP${suff}_VC9)
ELSE(WIN32)
  SET(CADSURF_LIBS_PATHS ${CADSURF_LIBS_PATHS} ${MESHGEMS_ROOT_DIR}/lib/Linux${suff})
ENDIF(WIN32)

FIND_LIBRARY(mg-cadsurf mg-cadsurf PATHS ${CADSURF_LIBS_PATHS})
FIND_LIBRARY(mg-precad mg-precad PATHS ${CADSURF_LIBS_PATHS})
FIND_LIBRARY(meshgems meshgems PATHS ${CADSURF_LIBS_PATHS})

SET(MESHGEMS_LIBRARIES)
SET(MESHGEMS_LIBRARIES ${MESHGEMS_LIBRARIES} ${mg-cadsurf})
SET(MESHGEMS_LIBRARIES ${MESHGEMS_LIBRARIES} ${mg-precad})
IF(meshgems)
  SET(MESHGEMS_LIBRARIES ${MESHGEMS_LIBRARIES} ${meshgems})
ENDIF(meshgems)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MESHGEMS REQUIRED_VARS MESHGEMS_INCLUDE_DIRS MESHGEMS_LIBRARIES)
