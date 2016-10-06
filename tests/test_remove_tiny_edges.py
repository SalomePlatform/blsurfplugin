# -*- coding: utf-8 -*-

import salome

import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

import math

tiny_edge_length = 0.1

O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)

Face_1 = geompy.MakeFaceHW(10, 10, 1)
geomObj_1 = geompy.MakeMarker(0, 0, 0, 1, 0, 0, 0, 1, 0)
Sketch_1 = geompy.MakeSketcherOnPlane("Sketcher:F -2.5209155082703 -1.4416453838348:TT -1.1105282306671 -2.9872753620148:TT 0.76354801654816 -2.3303825855255:TT 1.9614112377167 -3.0838770866394:TT 3.8354876041412 -1.2677619457245:TT 4.2218952178955 0.644955098629:TT 3.2751967906952 2.5576722621918:TT 0.58966463804245 3.5430111885071:TT -3.7380990982056 3.2338852882385:TT -4.433632850647 0.85747921466827:WW", Face_1 )
vertices = geompy.ExtractShapes(Sketch_1, geompy.ShapeType["VERTEX"], True)
Curve_1 = geompy.MakeInterpol(vertices, True, True)

p_small = geompy.MakeVertexOnCurve(Curve_1, 0.001)
geompy.addToStudy(p_small, "p_small")

part = geompy.MakePartition([Face_1], [Curve_1, p_small], Limit=geompy.ShapeType["FACE"])
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

faces = geompy.SubShapeAll(part, geompy.ShapeType["FACE"])
sub_shapes = []
for i, face in enumerate(faces):
  geompy.addToStudyInFather(part, face, "face_%i"%(i+1))
  sub_shapes.append(face)

# Mesh
# ====

import SMESH
from salome.smesh import smeshBuilder
smesh = smeshBuilder.New(salome.myStudy)

Mesh = smesh.Mesh(part, "Mesh")

algo2d = Mesh.Triangle(algo=smeshBuilder.BLSURF)
params = algo2d.Parameters()
params.SetVerbosity(1)
#algo2d.SetTopology(smesh.PreCAD)
params.SetGeometricMesh( 1 )
params.SetAngleMesh( 4 )
params.SetPhySize( 8 )
#algo2d.SetGradation(1.05)
#params.SetQuadAllowed( True )
params.SetQuadraticMesh( True )
params.SetRemoveTinyEdges( True )
params.SetTinyEdgeLength( tiny_edge_length )
#params.SetOptionValue( 'rectify_jacobian', '0' )

Mesh.Compute()

for sub_shape in sub_shapes:
  Mesh.Group(sub_shape)

x0, y0, z0 = geompy.PointCoordinates(p_small)
node = Mesh.FindNodeClosestTo(x0, y0, z0)

elems = Mesh.GetNodeInverseElements(node)

edges_length = []
for elem in elems:
    if Mesh.GetElementType(elem, True) == SMESH.EDGE:
        edge_length = Mesh.GetLength(elem)
        edges_length.append(edge_length)

min_length = min(edges_length)
x1, y1, z1 = Mesh.GetNodeXYZ(node)

print min_length

if min_length<tiny_edge_length:
    raise Exception("Small edge has not been removed")

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(True)
