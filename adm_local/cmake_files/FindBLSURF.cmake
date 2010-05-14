#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

SET(BLSURFHOME $ENV{BLSURFHOME})
FIND_PATH(BLSURF_INCLUDES_DIR distene/blsurf.h ${BLSURFHOME}/include)
SET(BLSURF_INCLUDES)
SET(BLSURF_INCLUDES ${BLSURF_INCLUDES} -I${BLSURF_INCLUDES_DIR})

FIND_LIBRARY(BLSurf BLSurf PATHS ${BLSURFHOME}/lib ${BLSURFHOME}/lib/WinXP_VC9)
FIND_LIBRARY(distene distene PATHS ${BLSURFHOME}/lib ${BLSURFHOME}/lib/WinXP_VC9)

SET(BLSURF_LIBS)
SET(BLSURF_LIBS ${BLSURF_LIBS} ${BLSurf})
IF(distene)
  SET(BLSURF_LIBS ${BLSURF_LIBS} ${distene})
ENDIF(distene)
