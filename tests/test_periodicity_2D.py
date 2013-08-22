# -*- coding: iso-8859-1 -*-

import salome

import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

import math

O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)

Face_1 = geompy.MakeFaceHW(10, 10, 1)
geomObj_1 = geompy.MakeMarker(0, 0, 0, 1, 0, 0, 0, 1, 0)
Sketch_1 = geompy.MakeSketcherOnPlane("Sketcher:F -2.5209155082703 -1.4416453838348:TT -1.1105282306671 -2.9872753620148:TT 0.76354801654816 -2.3303825855255:TT 1.9614112377167 -3.0838770866394:TT 3.8354876041412 -1.2677619457245:TT 4.2218952178955 0.644955098629:TT 3.2751967906952 2.5576722621918:TT 0.58966463804245 3.5430111885071:TT -3.7380990982056 3.2338852882385:TT -4.433632850647 0.85747921466827:WW", Face_1 )
vertices = geompy.ExtractShapes(Sketch_1, geompy.ShapeType["VERTEX"], True)
Curve_1 = geompy.MakeInterpol(vertices, True, True)

part = geompy.MakePartition([Face_1], [Curve_1], Limit=geompy.ShapeType["FACE"])
geompy.addToStudy(part, "part")

Vx = geompy.MakeVectorDXDYDZ(1, 0, 0)
Vy = geompy.MakeVectorDXDYDZ(0, 1, 0)
Vz = geompy.MakeVectorDXDYDZ(0, 0, 1)

p1 = geompy.MakeVertex(-5, -5, 0)
p2 = geompy.MakeVertex(5, 5, 0)
left_edges = geompy.GetShapesOnPlaneWithLocation(part, geompy.ShapeType["EDGE"], Vx, p1, GEOM.ST_ON)
left = geompy.CreateGroup(part, geompy.ShapeType["EDGE"])
geompy.UnionList(left, left_edges)
geompy.addToStudyInFather(part, left, "left")

right_edges = geompy.GetShapesOnPlaneWithLocation(part, geompy.ShapeType["EDGE"], Vx, p2, GEOM.ST_ON)
right = geompy.CreateGroup(part, geompy.ShapeType["EDGE"])
geompy.UnionList(right, right_edges)
geompy.addToStudyInFather(part, right, "right")

bottom_edges = geompy.GetShapesOnPlaneWithLocation(part, geompy.ShapeType["EDGE"], Vy, p1, GEOM.ST_ON)
bottom = geompy.CreateGroup(part, geompy.ShapeType["EDGE"])
geompy.UnionList(bottom, bottom_edges)
geompy.addToStudyInFather(part, bottom, "bottom")

top_edges = geompy.GetShapesOnPlaneWithLocation(part, geompy.ShapeType["EDGE"], Vy, p2, GEOM.ST_ON)
top = geompy.CreateGroup(part, geompy.ShapeType["EDGE"])
geompy.UnionList(top, top_edges)
geompy.addToStudyInFather(part, top, "top")


# Mesh
# ====

import SMESH
from salome.smesh import smeshBuilder
smesh = smeshBuilder.New(salome.myStudy)

Mesh = smesh.Mesh(part, "Mesh")

algo2d = Mesh.Triangle(algo=smeshBuilder.BLSURF)
algo2d.SetVerbosity(10)
#algo2d.SetTopology(smesh.PreCAD)
algo2d.SetGeometricMesh( 1 )
algo2d.SetAngleMesh( 4 )
algo2d.SetPhySize( 8 )
#algo2d.SetGradation(1.05)
algo2d.SetOptionValue( 'periodic_tolerance', '1e-2' )

# Periodicity

def proj_x(shape1):
    shape2 = geompy.MakeTranslation(shape1, 10, 0., 0)
    return shape2
    
def proj_y(shape1):
    shape2 = geompy.MakeTranslation(shape1, 0, 10., 0)
    return shape2

def AddAdvancedEdgesPeriodicity(edges1, edges2, f_transf):
    source_edges = geompy.SubShapeAll(edges1, geompy.ShapeType["EDGE"])
    j = 0
    k = 0
    for source_edge in source_edges:
        geompy.addToStudyInFather(edges1, source_edge, "source_edge_%i"%j)
        p_source = geompy.MakeVertexOnCurve(source_edge, 0.5)
        p_target = f_transf(p_source)
        target_edge = geompy.GetEdgeNearPoint(part, p_target)
        geompy.addToStudyInFather(edges2, target_edge, "target_edge_%i"%j)
        algo2d.AddEdgePeriodicityWithoutFaces(source_edge, target_edge)
        
        j += 1
        
        source_vertices = geompy.SubShapeAll(source_edge, geompy.ShapeType["VERTEX"])
        for source_vertex in source_vertices:
            geompy.addToStudyInFather(edges1, source_vertex, "source_vertex_%i"%k)
            target_vertex_tmp = geompy.MakeTranslation(source_vertex, 10, 0., 0)
            target_vertex_tmp = f_transf(source_vertex)
            target_vertex = geompy.GetSame(part, target_vertex_tmp)
            geompy.addToStudyInFather(edges2, target_vertex, "target_vertex_%i"%k)
            algo2d.AddVertexPeriodicity(source_edge, source_vertex, target_edge, target_vertex)
            
            k += 1
    
AddAdvancedEdgesPeriodicity(left, right, proj_x)
AddAdvancedEdgesPeriodicity(bottom, top, proj_y)

Mesh.Compute()

gr_left = Mesh.Group(left)
gr_right = Mesh.Group(right)
gr_bottom = Mesh.Group(bottom)
gr_top = Mesh.Group(top)

Mesh.Compute()

left_translated = Mesh.TranslateObjectMakeMesh( gr_left, SMESH.DirStruct( SMESH.PointStruct ( 10, 0, 0 )), 0, 'left_translated' )
bottom_translated = Mesh.TranslateObjectMakeMesh( gr_bottom, SMESH.DirStruct( SMESH.PointStruct ( 0, 10, 0 )), 0, 'bottom_translated' )

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
checkProjection(gr_top, bottom_translated)

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(1)
