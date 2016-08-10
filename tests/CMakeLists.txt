# Copyright (C) 2013-2016  CEA/DEN, EDF R&D
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

SET(BLSURFPLUGIN_TEST_FILES
  test_enforced_internal_vertex.py
  test_enforced_vertex.py
  test_periodicity_2D_precad.py
  test_periodicity_precad.py
  test_periodicity_reflexion_2D_precad.py
  test_periodicity_reflexion_precad.py
  test_periodicity_with_points_2D_precad.py
  test_precad_intersections.py
  test_precad_keep_gaps.py
  test_proximity.py
  test_quadrangles_gradation.py
  test_remove_tiny_edges.py
  test_sphere.py
)

INSTALL(FILES ${BLSURFPLUGIN_TEST_FILES} DESTINATION ${SALOME_BLSURFPLUGIN_INSTALL_TESTS})
