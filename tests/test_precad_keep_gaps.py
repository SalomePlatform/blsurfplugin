# -*- coding: utf-8 -*-

import sys
import salome

salome.salome_init()
theStudy = salome.myStudy

###
### GEOM component
###

import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)


geompy.init_geom(theStudy)

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

BLSURF_1 = Mesh_1.Triangle(algo=smeshBuilder.MG_CADSurf)
BLSURF_Parameters = BLSURF_1.Parameters()
BLSURF_Parameters.SetPhySize( 5 )

Mesh_1.Compute()

# Check that vertices are not merged by preCAD preprocessing
# when no preprocessing option has been explicitely called
nodes = []
for p in vertices:
    x, y, z = geompy.PointCoordinates(p)
    id_node = Mesh_1.FindNodeClosestTo(x, y, z)
    nodes.append(id_node)

nodes = list(set(nodes))

Mesh_1.MakeGroupByIds("nodes", SMESH.NODE, nodes)

assert len(nodes) == 3, "We should have 3 nodes. We got %i. => The preprocessing has done something, but we did not ask him to."%len(nodes)

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(True)
