# Copyright (C) 2013-2024  CEA, EDF
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

SALOME_CONFIGURE_FILE(doxyfile.in doxyfile)
SALOME_CONFIGURE_FILE(doxyfile_py.in doxyfile_py)
SALOME_CONFIGURE_FILE(static/header.html.in ${CMAKE_CURRENT_BINARY_DIR}/static/header.html)
SALOME_CONFIGURE_FILE(static/header_py.html.in ${CMAKE_CURRENT_BINARY_DIR}/static/header_py.html)

SET(smesh_file "$(SMESH_ROOT_DIR)/bin/salome/collect_mesh_methods.py")
SET(kernel_file "${KERNEL_ROOT_DIR}/bin/salome/prepare_generating_doc.py")

SET(ADD_VAR)
LIST(APPEND ADD_VAR SMESH_MeshersList=BLSURFPlugin)

SET(_cmd_options ${smesh_file} -d -o tmp/smeshBuilder.py BLSURFPlugin)
SALOME_GENERATE_ENVIRONMENT_SCRIPT(_cmd env_script "${PYTHON_EXECUTABLE}" "${_cmd_options}" CONTEXT "BLSURFPLUGIN_DOC" ADDITIONAL_VARIABLES ${ADD_VAR})

ADD_CUSTOM_TARGET(usr_docs ${CMAKE_COMMAND} -E make_directory tmp
  COMMAND ${PYTHON_EXECUTABLE} ${kernel_file} -o tmp/BLSURFPluginBuilder.py ${CMAKE_SOURCE_DIR}/src/BLSURFPlugin/BLSURFPluginBuilder.py
  COMMAND ${PYTHON_EXECUTABLE} ${kernel_file} -o tmp/smesh_algorithm.py ${SMESH_ROOT_DIR}/${SALOME_INSTALL_PYTHON}/salome/smesh/smesh_algorithm.py
  COMMAND ${_cmd}
  COMMAND ${DOXYGEN_EXECUTABLE} doxyfile_py
  COMMAND ${DOXYGEN_EXECUTABLE} doxyfile
  COMMAND ${CMAKE_COMMAND} -E remove_directory tmp   
  VERBATIM
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}            
)

INSTALL(CODE "EXECUTE_PROCESS(COMMAND \"${CMAKE_COMMAND}\" --build ${PROJECT_BINARY_DIR} --target usr_docs)")
INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/BLSURFPLUGIN DESTINATION ${SALOME_INSTALL_DOC}/gui)

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES BLSURFPLUGIN)
