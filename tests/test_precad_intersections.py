# -*- coding: iso-8859-1 -*-

###
### This file is generated automatically by SALOME v6.6.0 with dump python functionality
###

import sys
import salome

salome.salome_init()
theStudy = salome.myStudy

import salome_notebook
notebook = salome_notebook.notebook
sys.path.insert( 0, r'/export/home/bourcier/salome/tests')

###
### GEOM component
###

import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)


geompy.init_geom(theStudy)

geomObj_1 = geompy.MakeBoxDXDYDZ(100, 100, 100)
geomObj_2 = geompy.MakeVertex(-2, 60, 40)
geomObj_3 = geompy.MakeSpherePntR(geomObj_2, 10)
geomObj_4 = geompy.MakeVertex(70, 20, 60)
geomObj_5 = geompy.MakeSpherePntR(geomObj_4, 15)
geomObj_6 = geompy.MakeCompound([geomObj_3, geomObj_5])
geomObj_7 = geompy.GetInPlaceByHistory(geomObj_6, geomObj_3)
geomObj_8 = geompy.GetInPlaceByHistory(geomObj_6, geomObj_5)
Face_1 = geompy.MakeFaceHW(10, 10, 1)
Translation_1 = geompy.MakeTranslation(Face_1, 10.0001, 0.0001, 0)
Translation_2 = geompy.MakeTranslation(Face_1, 5, -9.99995, 0)
Partition_1 = geompy.MakePartition([Face_1, Translation_1, Translation_2], [], [], [], geompy.ShapeType["FACE"], 0, [], 0)
geompy.addToStudy( Face_1, 'Face_1' )
geompy.addToStudy( Translation_1, 'Translation_1' )
geompy.addToStudy( Translation_2, 'Translation_2' )
geompy.addToStudy( Partition_1, 'Partition_1' )

p_axe = geompy.MakeVertex(5, -5, 0)
axe = geompy.MakePrismDXDYDZ(p_axe, 0, 0, 1)
vertices = geompy.GetShapesOnCylinder(Partition_1, geompy.ShapeType["VERTEX"], axe, 1e-3, GEOM.ST_IN)
gr_vertices = geompy.CreateGroup(Partition_1, geompy.ShapeType["VERTEX"])
geompy.UnionList(gr_vertices, vertices)
geompy.addToStudyInFather(Partition_1, gr_vertices, "vertices")

###
### SMESH component
###

import SMESH
from salome.smesh import smeshBuilder
smesh = smeshBuilder.New(salome.myStudy)


Mesh_1 = smesh.Mesh(Partition_1)

BLSURF_1 = Mesh_1.Triangle(algo=smeshBuilder.BLSURF)
BLSURF_Parameters = BLSURF_1.Parameters()
BLSURF_Parameters.SetPhySize( 5 )
BLSURF_Parameters.SetTopology( 3 )

Mesh_1.Compute()

# Check that vertices are merged by preCAD preprocessing
nodes = []
for p in vertices:
    x, y, z = geompy.PointCoordinates(p)
    id_node = Mesh_1.FindNodeClosestTo(x, y, z)
    nodes.append(id_node)

nodes = set(nodes)

assert(len(nodes) == 1)

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(1)
