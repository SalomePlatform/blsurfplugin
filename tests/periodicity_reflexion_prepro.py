#!/usr/bin/env python
# -*- coding: utf-8 -*-

import salome
import math

import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

simple = False

r = 10
dist = 10

p1 = geompy.MakeVertex(0., 0., 0.)
p2 = geompy.MakeVertex(100., 100., 100.)
box = geompy.MakeBoxTwoPnt(p1, p2)
geompy.addToStudy(box, "box")

p3 = geompy.MakeVertex(50., 5., 25.)
sphere1 = geompy.MakeSpherePntR(p3, 15.)
geompy.addToStudy(sphere1, "sphere1")

Vx = geompy.MakeVectorDXDYDZ(1, 0, 0)
Vy = geompy.MakeVectorDXDYDZ(0, 1, 0)
Vz = geompy.MakeVectorDXDYDZ(0, 0, 1)

p4 = geompy.MakeVertex(100., 0., 0.)
axe = geompy.MakePrismVecH(p4, Vz, 1)
geompy.addToStudy(axe, "axe")

sphere1_rota = geompy.MakeRotation(sphere1, axe, -math.pi/2.)
geompy.addToStudy(sphere1_rota, "sphere1_rota")

part = geompy.MakePartition([box], [sphere1, sphere1_rota])
geompy.addToStudy(part, "part")

left_faces = geompy.GetShapesOnPlane(part, geompy.ShapeType["FACE"], Vy, GEOM.ST_ON)
left = geompy.CreateGroup(part, geompy.ShapeType["FACE"])
geompy.UnionList(left, left_faces)
geompy.addToStudyInFather(part, left, "left")

right_faces = geompy.GetShapesOnPlaneWithLocation(part, geompy.ShapeType["FACE"], Vy, p2, GEOM.ST_ON)
right = geompy.CreateGroup(part, geompy.ShapeType["FACE"])
geompy.UnionList(right, right_faces)
geompy.addToStudyInFather(part, right, "right")

back_faces = geompy.GetShapesOnPlane(part, geompy.ShapeType["FACE"], Vx, GEOM.ST_ON)
back = geompy.CreateGroup(part, geompy.ShapeType["FACE"])
geompy.UnionList(back, back_faces)
geompy.addToStudyInFather(part, back, "back")

front_faces = geompy.GetShapesOnPlaneWithLocation(part, geompy.ShapeType["FACE"], Vx, p2, GEOM.ST_ON)
front = geompy.CreateGroup(part, geompy.ShapeType["FACE"])
geompy.UnionList(front, front_faces)
geompy.addToStudyInFather(part, front, "front")

bottom_faces = geompy.GetShapesOnPlane(part, geompy.ShapeType["FACE"], Vz, GEOM.ST_ON)
bottom = geompy.CreateGroup(part, geompy.ShapeType["FACE"])
geompy.UnionList(bottom, bottom_faces)
geompy.addToStudyInFather(part, bottom, "bottom")

top_faces = geompy.GetShapesOnPlaneWithLocation(part, geompy.ShapeType["FACE"], Vz, p2, GEOM.ST_ON)
top = geompy.CreateGroup(part, geompy.ShapeType["FACE"])
geompy.UnionList(top, top_faces)
geompy.addToStudyInFather(part, top, "top")

p1_rota = geompy.MakeRotation(p1, axe, -math.pi/2.)
geompy.addToStudy(sphere1_rota, "sphere1_rota")

p5 = geompy.MakeVertex(100, 0, 100)
geompy.addToStudy(p5, "p5")

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

# Periodicity
#algo2d.SetVerbosity(10)
#algo2d.SetPreCADOptionValue("periodic_tolerance", "1e-2")
algo2d.AddPreCadFacesPeriodicity(left, front, [p1, p4, p5], [p1_rota, p4, p5])


gr_left = Mesh.Group(left)
gr_right = Mesh.Group(right)
gr_front = Mesh.Group(front)
gr_back = Mesh.Group(back)
gr_bottom = Mesh.Group(bottom)
gr_top = Mesh.Group(top)

Mesh.Compute()

left_rotated = Mesh.RotateObjectMakeMesh( gr_left, axe, -math.pi/2, NewMeshName='left_rotated' )

def checkProjection(gr, mesh_translated, tol=1e-7):
    name = gr.GetName() + "_" + mesh_translated.GetName().split("_")[0]
    mesh_source = smesh.CopyMesh(gr, gr.GetName())
    mesh_check = smesh.Concatenate([mesh_source.GetMesh(), mesh_translated.GetMesh()], 0, name=name)
    ll_coincident_nodes = mesh_check.FindCoincidentNodes(tol)
    coincident_nodes = [item for sublist in ll_coincident_nodes for item in sublist]
    mesh_check.MakeGroupByIds("coincident_nodes", SMESH.NODE, coincident_nodes)
    mesh_nodes = mesh_check.GetNodesId()
    if len(ll_coincident_nodes) != mesh_translated.NbNodes():
        non_coincident_nodes = list(set(mesh_nodes) - set(coincident_nodes))
        mesh_check.MakeGroupByIds("non_coincident_nodes", SMESH.NODE, non_coincident_nodes)
        raise Exception("Projection failed for %s"%name)
        
checkProjection(gr_front, left_rotated)

salome.sg.updateObjBrowser(True)

