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

FIND_PATH(MESHGEMS_INCLUDE_DIRS meshgems/cadsurf.h HINTS ${MESHGEMS_ROOT_DIR} PATH_SUFFIXES include)

IF(MACHINE_IS_64)
  SET(_suff "_64")
ELSE()
  SET(_suff)
ENDIF(MACHINE_IS_64)
IF(WIN32)	
  SET(_plt WinXP${_suff}_VC9 WinXP${_suff}_VC10)
ELSE()
  SET(_plt Linux${_suff})
ENDIF(WIN32)

SET(CADSURF_LIBS_PATHS ${MESHGEMS_ROOT_DIR}/lib)

FIND_LIBRARY(mg-cadsurf mg-cadsurf PATHS ${CADSURF_LIBS_PATHS} PATH_SUFFIXES ${_plt})
FIND_LIBRARY(mg-precad mg-precad PATHS ${CADSURF_LIBS_PATHS} PATH_SUFFIXES ${_plt})
FIND_LIBRARY(meshgems meshgems PATHS ${CADSURF_LIBS_PATHS} PATH_SUFFIXES ${_plt})

# TODO: search all components
SET(MESHGEMS_LIBRARIES)
SET(MESHGEMS_LIBRARIES ${MESHGEMS_LIBRARIES} ${mg-cadsurf})
SET(MESHGEMS_LIBRARIES ${MESHGEMS_LIBRARIES} ${mg-precad})
IF(meshgems)
  SET(MESHGEMS_LIBRARIES ${MESHGEMS_LIBRARIES} ${meshgems})
ENDIF(meshgems)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MESHGEMS REQUIRED_VARS MESHGEMS_INCLUDE_DIRS MESHGEMS_LIBRARIES)
