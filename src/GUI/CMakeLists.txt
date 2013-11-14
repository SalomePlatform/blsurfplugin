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

INCLUDE(UseQt4Ext)

# --- options ---
# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${CAS_INCLUDE_DIRS}
  ${PYTHON_INCLUDES}
  ${KERNEL_INCLUDE_DIRS}
  ${GUI_INCLUDE_DIRS}
  ${GEOM_INCLUDE_DIRS}
  ${SMESH_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${MESHGEMS_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_SOURCE_DIR}/src/BLSURFPlugin
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  ${CAS_DEFINITIONS}
  ${BOOST_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES
  ${QT_LIBRARIES}
  ${QT_MT_LIBS}
  ${GUI_qtx}
  ${GUI_SalomeApp}
  ${GUI_suit}
  ${GUI_SalomeObject}
  ${GUI_LightApp}
  ${GEOM_GEOM}
  ${SMESH_SMESH}
  ${SMESH_SMESHEngine}
  ${SMESH_GeomSelectionTools}
  ${SMESH_StdMeshersGUI}
  ${SMESH_SMESHFiltersSelection}
  ${CAS_KERNEL}
  ${MESHGEMS_LIBRARIES}
  SalomeIDLBLSURFPLUGIN
  BLSURFEngine
)

# --- headers ---

# header files / to be processed by moc
SET(BLSURFPluginGUI_HEADERS
  BLSURFPluginGUI_HypothesisCreator.h
  BLSURFPluginGUI_Dlg.h
)

# --- sources ---

# sources / moc wrappings
QT4_WRAP_CPP(_moc_SOURCES ${BLSURFPluginGUI_HEADERS})

# sources / static
SET(_other_SOURCES
  BLSURFPluginGUI.cxx
  BLSURFPluginGUI_StdWidget.cxx
  BLSURFPluginGUI_AdvWidget.cxx
  BLSURFPluginGUI_HypothesisCreator.cxx
)

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  BLSURFPlugin_images.ts
  BLSURFPlugin_msg_en.ts
  BLSURFPlugin_msg_fr.ts
  BLSURFPlugin_msg_ja.ts
) 

# resource files / to be processed by uic
SET(_uic_files
  BLSURFPluginGUI_StdWidget_QTD.ui
  BLSURFPluginGUI_AdvWidget_QTD.ui
)

# header files / uic wrappings
QT4_WRAP_UI(_uic_HEADERS ${_uic_files})

# sources / to compile
SET(BLSURFPluginGUI_SOURCES ${_other_SOURCES} ${_moc_SOURCES} ${_uic_HEADERS})

# --- rules ---

ADD_LIBRARY(BLSURFPluginGUI ${BLSURFPluginGUI_SOURCES})
TARGET_LINK_LIBRARIES(BLSURFPluginGUI ${_link_LIBRARIES} )
INSTALL(TARGETS BLSURFPluginGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${BLSURFPluginGUI_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

QT4_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_BLSURFPLUGIN_INSTALL_RES_DATA}")