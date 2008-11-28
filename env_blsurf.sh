#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
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
#------ Licence Distene ------
#
export DISTENE_LICENSE_FILE=${INSTALL_ROOT}/dlim8.key
##
#------ BLSURF ------
export BLSURFHOME=${INSTALL_ROOT/BLSurf
##
#------ BLSURFplugin-Src ------
export BLSURFPLUGIN_SRC_DIR=${INSTALL_ROOT}/BLSURFPLUGIN_SRC_3.2.4
##
#------ BLSURFplugin-Bin ------
export BLSURFPLUGIN_ROOT_DIR=${INSTALL_ROOT}/BLSURFPLUGIN_INSTALL
if [ -n "${ENV_FOR_LAUNCH}" ] ; then
  if [ "${ENV_FOR_LAUNCH}" == "1" ] ; then
    exportp LD_LIBRARY_PATH ${BLSURFPLUGIN_ROOT_DIR}/lib/salome:${BLSURFHOME}/lib
    exportp PYTHONPATH ${BLSURFPLUGIN_ROOT_DIR}/lib64/salome:${BLSURFPLUGIN_ROOT_DIR}/lib64/python${PYTHON_VERSION}/site-packages/salome
  fi
fi
##
