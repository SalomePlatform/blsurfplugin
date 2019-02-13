# -*- coding: utf-8 -*-
# Copyright (C) 2015-2019  CEA/DEN, EDF R&D
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


import sys
import salome

salome.salome_init()
theStudy = salome.myStudy

###
### GEOM component
###

import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

import math
import SALOMEDS


geompy = geomBuilder.New(theStudy)

h = 5

O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
Cylinder_1 = geompy.MakeCylinderRH(1, h)
Sphere_1 = geompy.MakeSphereR(1.5)
Translation_1 = geompy.MakeTranslation(Sphere_1, 0, 0, h)
Fuse_1 = geompy.MakeFuseList([Cylinder_1, Sphere_1, Translation_1], False, True)
geompy.addToStudy( O, 'O' )
geompy.addToStudy( OX, 'OX' )
geompy.addToStudy( OY, 'OY' )
geompy.addToStudy( OZ, 'OZ' )
geompy.addToStudy( Cylinder_1, 'Cylinder_1' )
geompy.addToStudy( Sphere_1, 'Sphere_1' )
geompy.addToStudy( Translation_1, 'Translation_1' )
geompy.addToStudy( Fuse_1, 'Fuse_1' )

###
### SMESH component
###

import  SMESH, SALOMEDS
from salome.smesh import smeshBuilder

from salome.BLSURFPlugin import BLSURFPluginBuilder

smesh = smeshBuilder.New()

# First mesh with quadrangle-dominant gradation
Mesh_1 = smesh.Mesh(Fuse_1, "Mesh_1")
MG_CADSurf = Mesh_1.Triangle(algo=smeshBuilder.MG_CADSurf)
MG_CADSurf_Parameters_1 = MG_CADSurf.Parameters()
MG_CADSurf_Parameters_1.SetElementType( BLSURFPluginBuilder.QuadrangleDominant )
MG_CADSurf_Parameters_1.SetPhySize( 1 )
MG_CADSurf_Parameters_1.SetMaxSize( 1 )
MG_CADSurf_Parameters_1.SetGradation( 1.05 )
MG_CADSurf_Parameters_1.SetAngleMesh( 4 )
isDone = Mesh_1.Compute()

min_1, max_1 = Mesh_1.GetMinMax(SMESH.FT_Area)

# Check that min and max areas are not too far
assert max_1/min_1 < 6

# Second mesh with anisotropy (which disable gradation)
Mesh_2 = smesh.Mesh(Fuse_1, "Mesh_2")
MG_CADSurf = Mesh_2.Triangle(algo=smeshBuilder.MG_CADSurf)
MG_CADSurf_Parameters_2 = MG_CADSurf.Parameters()
MG_CADSurf_Parameters_2.SetElementType( BLSURFPluginBuilder.QuadrangleDominant )
MG_CADSurf_Parameters_2.SetPhySize( 1 )
MG_CADSurf_Parameters_2.SetMaxSize( 1 )
MG_CADSurf_Parameters_2.SetGradation( 1.05 )
MG_CADSurf_Parameters_2.SetAngleMesh( 4 )
MG_CADSurf_Parameters_2.SetAnisotropic( True )
isDone = Mesh_2.Compute()

min_2, max_2 = Mesh_2.GetMinMax(SMESH.FT_Area)

# Check that min and max ration increase with anisotropy
assert max_2/min_2 > 20

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()


