# -*- coding: utf-8 -*-

import sys
import salome

salome.salome_init()

###
### GEOM component
###

import GEOM
from salome.geom import geomBuilder
import math
import SALOMEDS


geompy = geomBuilder.New()

O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
Face_1 = geompy.MakeFaceHW(100, 100, 1)
Disk_1 = geompy.MakeDiskR(20, 1)
Partition_1 = geompy.MakePartition([Face_1, Disk_1], Limit=geompy.ShapeType["FACE"])
geompy.addToStudy( O, 'O' )
geompy.addToStudy( OX, 'OX' )
geompy.addToStudy( OY, 'OY' )
geompy.addToStudy( OZ, 'OZ' )
geompy.addToStudy( Face_1, 'Face_1' )
geompy.addToStudy( Disk_1, 'Disk_1' )
geompy.addToStudy( Partition_1, 'Partition_1' )

###
### SMESH component
###

import  SMESH, SALOMEDS
from salome.smesh import smeshBuilder

from salome.BLSURFPlugin import BLSURFPluginBuilder

smesh = smeshBuilder.New()
Mesh_1 = smesh.Mesh(Partition_1)
MG_CADSurf = Mesh_1.Triangle(algo=smeshBuilder.MG_CADSurf)
MG_CADSurf_Parameters_1 = MG_CADSurf.Parameters()
MG_CADSurf_Parameters_1.SetPhySize( 10 )
MG_CADSurf_Parameters_1.SetMaxSize( 10 )
MG_CADSurf_Parameters_1.SetChordalError( -1 )


# Test quadrangle dominant
MG_CADSurf_Parameters_1.SetElementType( BLSURFPluginBuilder.QuadrangleDominant )
isDone = Mesh_1.Compute()

assert Mesh_1.NbQuadrangles() > 0
assert Mesh_1.NbTriangles() > 0
assert Mesh_1.NbQuadrangles() > Mesh_1.NbTriangles()


# Test quadrangles only
MG_CADSurf_Parameters_1.SetElementType( BLSURFPluginBuilder.Quadrangles )
isDone = Mesh_1.Compute()

assert Mesh_1.NbQuadrangles() > 0
assert Mesh_1.NbTriangles() == 0


# Test triangles only
MG_CADSurf_Parameters_1.SetElementType( BLSURFPluginBuilder.Triangles )
isDone = Mesh_1.Compute()

assert Mesh_1.NbQuadrangles() == 0
assert Mesh_1.NbTriangles() > 0

# Test quadrangle dominant compatibility (to be deprecated in Salome 9)
MG_CADSurf_Parameters_1.SetQuadAllowed( True )
isDone = Mesh_1.Compute()

assert Mesh_1.NbQuadrangles() > 0
assert Mesh_1.NbTriangles() > 0
assert Mesh_1.NbQuadrangles() > Mesh_1.NbTriangles()

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(True)
