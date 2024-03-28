# Copyright (C) 2016-2024  CEA, EDF
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

INCLUDE(tests.set)

SET(COMPONENT_NAME BLSURFPLUGIN)
SET(MULTITHREAD_COMPONENT_NAME MULTITHREAD)
SET(SALOME_TEST_DRIVER "$ENV{KERNEL_ROOT_DIR}/bin/salome/appliskel/python_test_driver.py")
SET(TIMEOUT        300)


FOREACH(tfile ${TEST_NAMES} ${EXAMPLE_NAMES})
  SET(TEST_NAME ${COMPONENT_NAME}_${tfile})
  ADD_TEST(${TEST_NAME} python ${SALOME_TEST_DRIVER} ${TIMEOUT} ${tfile}.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES LABELS "${COMPONENT_NAME}")
ENDFOREACH()

FOREACH(tfile ${MULTITHREAD_TEST_NAMES})
  SET(MULTITHREAD_TEST_NAME ${MULTITHREAD_COMPONENT_NAME}_${tfile})
  ADD_TEST(${MULTITHREAD_TEST_NAME} python ${PYTHON_TEST_DRIVER} ${TIMEOUT} ${tfile}.py)
  SET_TESTS_PROPERTIES(${MULTITHREAD_TEST_NAME} PROPERTIES LABELS MULTITHREAD)
ENDFOREACH()