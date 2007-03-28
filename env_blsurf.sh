#------ Licence Distene ------
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
