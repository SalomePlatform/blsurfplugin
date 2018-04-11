# -*- coding: utf-8 -*-

import sys
import salome

salome.salome_init()
theStudy = salome.myStudy

import  SMESH, SALOMEDS

## Compute the minimum area of the faces of the mesh
def getMinArea(mesh):
  faces = mesh.GetElementsByType(SMESH.FACE)
  areas = [mesh.GetArea(face) for face in faces]
  return min(areas)

###
### GEOM component
###

import GEOM
from salome.geom import geomBuilder
import math
import SALOMEDS


geompy = geomBuilder.New(theStudy)

O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
Face_1 = geompy.MakeFaceHW(100, 100, 1)
Box_1 = geompy.MakePrismVecH(Face_1, OZ, -100)
# define the edge slightly longer than the face to test out of bounds case.
P1 = geompy.MakeVertex(-50.5, 0, 0)
P2 = geompy.MakeVertex(50.5, 0, 0)
Edge_1 = geompy.MakeEdge(P1, P2)
geompy.addToStudy( O, 'O' )
geompy.addToStudy( OX, 'OX' )
geompy.addToStudy( OY, 'OY' )
geompy.addToStudy( OZ, 'OZ' )
geompy.addToStudy( Face_1, 'Face_1' )
geompy.addToStudy( Box_1, 'Box_1' )
geompy.addToStudy( Edge_1, 'Edge_1' )

sub_Face_1 = geompy.GetInPlace(Box_1, Face_1)
geompy.addToStudyInFather(Box_1, sub_Face_1, "Face_1")

###
### SMESH component
###

from salome.smesh import smeshBuilder

smesh = smeshBuilder.New(theStudy)
Mesh_1 = smesh.Mesh(Box_1)
MG_CADSurf = Mesh_1.Triangle(algo=smeshBuilder.MG_CADSurf)
MG_CADSurf_Parameters_1 = MG_CADSurf.Parameters()
MG_CADSurf_Parameters_1.SetPhySize( 14.1421 )
MG_CADSurf_Parameters_1.SetMinSize( 0.141421 )
MG_CADSurf_Parameters_1.SetMaxSize( 28.2843 )
MG_CADSurf_Parameters_1.SetChordalError( 7.07107 )
#MG_CADSurf_Parameters_1.SetAttractorGeom( sub_Face_1, Edge_1, 1, 14.1421, 5, 5 )

Mesh_1.Compute()

min_area_without_attractor = getMinArea(Mesh_1)

print "min_area_without_attractor: ", min_area_without_attractor

MG_CADSurf_Parameters_1.SetAttractorGeom( sub_Face_1, Edge_1, 1, 14.1421, 5, 5 )

Mesh_1.Compute()

min_area_with_attractor = getMinArea(Mesh_1)

print "min_area_with_attractor: ", min_area_with_attractor

assert min_area_with_attractor < min_area_without_attractor

assert min_area_with_attractor < 1

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(True)