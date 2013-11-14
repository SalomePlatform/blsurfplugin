# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

# --- options ---
# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${PYTHON_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${GUI_INCLUDE_DIRS}
  ${GEOM_INCLUDE_DIRS}
  ${CAS_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${MESHGEMS_INCLUDE_DIRS}
  ${SMESH_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}/idl
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(  
  ${OMNIORB_DEFINITIONS}
  ${CAS_DEFINITIONS}
  ${BOOST_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES
  ${PYTHON_LIBRARIES}
  ${CAS_TKBRep}
  ${CAS_TKGeomBase}
  ${CAS_TKGeomAlgo}
  ${CAS_TKTopAlgo}
  ${CAS_TKXCAF}
  ${CAS_TKXSBase}
  ${CAS_TKG2d}
  ${CAS_TKG3d}
  ${CAS_TKShHealing}
  ${MESHGEMS_LIBRARIES}
  ${SMESH_SMESHimpl}
  ${SMESH_SMESHEngine}
  ${SMESH_StdMeshers}
  ${SMESH_StdMeshersEngine}
  ${SMESH_SMDS}
  ${SMESH_SMESHDS}
  ${GEOM_GEOMbasic}
  ${KERNEL_OpUtil}
  ${KERNEL_SalomeGenericObj}
  ${KERNEL_SalomeNS}
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_SALOMEBasics}
  ${Boost_LIBRARIES}
  SalomeIDLBLSURFPLUGIN
)

# --- headers ---

# header files
SET(BLSURFEngine_HEADERS
  BLSURFPlugin_BLSURF.hxx
  BLSURFPlugin_BLSURF_i.hxx
  BLSURFPlugin_Hypothesis.hxx
  BLSURFPlugin_Hypothesis_i.hxx
  BLSURFPlugin_Attractor.hxx
)

# --- sources ---

# sources / static
SET(BLSURFEngine_SOURCES
  BLSURFPlugin_BLSURF.cxx
  BLSURFPlugin_BLSURF_i.cxx
  BLSURFPlugin_Hypothesis.cxx
  BLSURFPlugin_Hypothesis_i.cxx
  BLSURFPlugin_i.cxx
  BLSURFPlugin_Attractor.cxx
)

# --- scripts ---

# scripts / static
SET(_bin_SCRIPTS
  __init__.py
  BLSURFPluginBuilder.py
)

# --- rules ---

ADD_LIBRARY(BLSURFEngine ${BLSURFEngine_SOURCES})
TARGET_LINK_LIBRARIES(BLSURFEngine ${_link_LIBRARIES} )
INSTALL(TARGETS BLSURFEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${BLSURFEngine_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

SALOME_INSTALL_SCRIPTS("${_bin_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/salome/BLSURFPlugin)