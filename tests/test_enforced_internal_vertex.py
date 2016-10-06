# -*- coding: utf-8 -*-

import salome
import math

import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

r = 10
dist = 10
dist_coin = 10.1

p1 = geompy.MakeVertex(0., 0., 0.)
p2 = geompy.MakeVertex(100., 100., 100.)
box = geompy.MakeBoxTwoPnt(p1, p2)
geompy.addToStudy(box, "box")

p3 = geompy.MakeVertex(dist_coin, 0, dist_coin)
geompy.addToStudy(p3, "p3")

part = geompy.MakePartition([box], [p3])
geompy.addToStudy(part, "part")

left = geompy.GetFaceNearPoint(box, p3)
geompy.addToStudyInFather(box, left, "left")


# Mesh
# ====

import SMESH
from salome.smesh import smeshBuilder
smesh = smeshBuilder.New(salome.myStudy)

Mesh = smesh.Mesh(part, "Mesh")

algo2d = Mesh.Triangle(algo=smeshBuilder.MG_CADSurf)
algo2d.SetGeometricMesh( 1 )
algo2d.SetAngleMesh( 4 )
algo2d.SetPhySize( 8 )

algo2d.SetInternalEnforcedVertexAllFaces(True)

Mesh.Compute()

id_node = Mesh.FindNodeClosestTo(dist_coin, 0, dist_coin)

x, y, z = Mesh.GetNodeXYZ(id_node)

assert("%.2f, %.2f, %.2f"%(x, y, z) == "%.2f, %.2f, %.2f"%(dist_coin, 0, dist_coin))

salome.sg.updateObjBrowser(True)

