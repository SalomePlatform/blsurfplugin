# -*- coding: utf-8 -*-

import salome

import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

import math

simple = False

r = 10
dist = 10

p1 = geompy.MakeVertex(0., 0., 0.)
p2 = geompy.MakeVertex(100., 100., 100.)
box = geompy.MakeBoxTwoPnt(p1, p2)
geompy.addToStudy(box, "box")

p3 = geompy.MakeVertex(25., 5., 25.)
sphere1 = geompy.MakeSpherePntR(p3, 15.)
geompy.addToStudy(sphere1, "sphere1")

sphere1_trans = geompy.MakeTranslation(sphere1, 0, 100, 0)
geompy.addToStudy(sphere1_trans, "sphere1_trans")

sphere1_trans = geompy.MakeTranslation(sphere1, 0, 100, 0)
geompy.addToStudy(sphere1_trans, "sphere1_trans")

p4 = geompy.MakeVertex(5, 50, 90)
sphere2 = geompy.MakeSpherePntR(p4, 20.)

sphere2_trans = geompy.MakeTranslation(sphere2, 100, 0, 0)
geompy.addToStudy(sphere2_trans, "sphere2_trans")

sphere2_trans2 = geompy.MakeTranslation(sphere2, 0, 0, -100)
geompy.addToStudy(sphere2_trans2, "sphere2_trans2")

sphere2_trans3 = geompy.MakeTranslation(sphere2, 100, 0, -100)
geompy.addToStudy(sphere2_trans3, "sphere2_trans3")

if simple:
    part = box
else:
    part = geompy.MakePartition([box], [sphere1, sphere1_trans, sphere2, sphere2_trans, sphere2_trans2, sphere2_trans3])
geompy.addToStudy(part, "part")

Vx = geompy.MakeVectorDXDYDZ(1, 0, 0)
Vy = geompy.MakeVectorDXDYDZ(0, 1, 0)
Vz = geompy.MakeVectorDXDYDZ(0, 0, 1)

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

sources = geompy.CreateGroup(part, geompy.ShapeType["FACE"])
geompy.UnionList(sources, left_faces)
geompy.UnionList(sources, back_faces)
geompy.UnionList(sources, top_faces)
geompy.addToStudyInFather(part, sources, "sources")


# Mesh
# ====

import SMESH
from salome.smesh import smeshBuilder
smesh = smeshBuilder.New(salome.myStudy)

Mesh = smesh.Mesh(part, "Mesh")

algo2d = Mesh.Triangle(algo=smeshBuilder.BLSURF)
algo2d.SetGeometricMesh( 1 )
algo2d.SetAngleMesh( 4 )
algo2d.SetPhySize( 8 )
algo2d.SetOptionValue( 'periodic_tolerance', '1e-2' )

def proj_x(shape1):
    shape2 = geompy.MakeTranslation(shape1, 100, 0., 0)
    return shape2
    
def proj_y(shape1):
    shape2 = geompy.MakeTranslation(shape1, 0, 100., 0)
    return shape2

def proj_z(shape1):
    shape2 = geompy.MakeTranslation(shape1, 0, 0, 100.)
    return shape2

def AddAdvancedFacesPeriodicity(faces1, faces2, f_transf):
    # Periodicity left/right
    source_faces = geompy.SubShapeAll(faces1, geompy.ShapeType["FACE"])
    i = 0
    j = 0
    k = 0
    for source_face in source_faces:
        geompy.addToStudyInFather(faces1, source_face, "source_face_%i"%i)
        p_source = geompy.MakeVertexInsideFace(source_face)
        p_target = f_transf(p_source)
        target_face = geompy.GetFaceNearPoint(faces2, p_target)
        geompy.addToStudyInFather(faces2, target_face, "target_face_%i"%i)
        algo2d.AddFacePeriodicity(source_face, target_face)
        i += 1
        
        source_edges = geompy.SubShapeAll(source_face, geompy.ShapeType["EDGE"])
        for source_edge in source_edges:
            geompy.addToStudyInFather(faces1, source_edge, "source_edge_%i"%(j))
            p_source = geompy.MakeVertexOnCurve(source_edge, 0.5)
            p_target = f_transf(p_source)
            target_edge = geompy.GetEdgeNearPoint(faces2, p_target)
            geompy.addToStudyInFather(faces2, target_edge, "target_edge_%i"%(j))
            algo2d.AddEdgePeriodicity(source_face, source_edge, target_face, target_edge)
            j += 1
            
            source_vertices = geompy.SubShapeAll(source_edge, geompy.ShapeType["VERTEX"])
            for source_vertex in source_vertices:
                geompy.addToStudyInFather(faces1, source_vertex, "source_vertex_%i"%(k))
                target_vertex_tmp = f_transf(source_vertex)
                target_vertex = geompy.GetSame(faces2, target_vertex_tmp)
                geompy.addToStudyInFather(faces2, target_vertex, "target_vertex_%i"%(k))
                algo2d.AddVertexPeriodicity(source_edge, source_vertex, target_edge, target_vertex)
                k += 1
    pass

AddAdvancedFacesPeriodicity(back, front, proj_x)
AddAdvancedFacesPeriodicity(left, right, proj_y)
AddAdvancedFacesPeriodicity(bottom, top, proj_z)

gr_left = Mesh.Group(left)
gr_right = Mesh.Group(right)
gr_front = Mesh.Group(front)
gr_back = Mesh.Group(back)
gr_bottom = Mesh.Group(bottom)
gr_top = Mesh.Group(top)

Mesh.Compute()

left_translated = Mesh.TranslateObjectMakeMesh( gr_left, SMESH.DirStruct( SMESH.PointStruct ( 0, 100, 0 )), 0, 'left_translated' )
front_translated = Mesh.TranslateObjectMakeMesh( gr_front, SMESH.DirStruct( SMESH.PointStruct ( -100, 0, 0 )), 0, 'front_translated' )
bottom_translated = Mesh.TranslateObjectMakeMesh( gr_bottom, SMESH.DirStruct( SMESH.PointStruct ( 0, 0, 100 )), 0, 'bottom_translated' )

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
        
checkProjection(gr_right, left_translated)
checkProjection(gr_back, front_translated)
checkProjection(gr_top, bottom_translated)

salome.sg.updateObjBrowser(0)
