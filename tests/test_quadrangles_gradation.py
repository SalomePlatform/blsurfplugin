# -*- coding: utf-8 -*-

import salome

import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

import math

area_tolerance = 1e-2
nb_faces_tolerance = 20

## Return the min and max areas of a mesh
def getMinMaxArea(mesh):
  mini, maxi = mesh.GetMinMax(SMESH.FT_Area)
  return mini, maxi

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

p_small2 = geompy.MakeVertexOnCurve(Curve_1, 0.5)
geompy.addToStudy(p_small2, "p_small2")

p_small3 = geompy.MakeVertexOnCurve(Curve_1, 0.501)
geompy.addToStudy(p_small3, "p_small3")

part = geompy.MakePartition([Face_1], [Curve_1, p_small, p_small2, p_small3], Limit=geompy.ShapeType["FACE"])
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

# Test gradation with quadrangles
Mesh_1 = smesh.Mesh(part, "Mesh")

algo2d = Mesh_1.Triangle(algo=smeshBuilder.BLSURF)
params = algo2d.Parameters()
params.SetGeometricMesh( 1 )
params.SetAngleMesh( 4 )
params.SetPhySize( 8 )
algo2d.SetGradation(1.05)
params.SetQuadAllowed( True )

ok = Mesh_1.Compute()

if not ok:
  raise Exception("Error when computing Mesh_1")

# Check the areas of Mesh_1
mini_1, maxi_1 = getMinMaxArea(Mesh_1)
mini_1_ref = 0.00197
maxi_1_ref = 0.530

if abs(mini_1-mini_1_ref) > area_tolerance:
  raise Exception("Min area of Mesh_1 incorrect")

if maxi_1 > maxi_1_ref:
  raise Exception("Max area of Mesh_1 incorrect")

# Check the number of faces of Mesh_1
nb_faces_1 = Mesh_1.NbFaces()
nb_faces_1_ref = 1208

if nb_faces_1 < nb_faces_1_ref:
  raise Exception("Number of faces of Mesh_1 incorrect")


# Test no gradation with quadrangles and anisotropy
# RQ: this kind of mesh is not suitable for simulation
# but gives a coarse mesh like a stl geometry
Mesh_2 = smesh.Mesh(part, "Mesh")

algo2d = Mesh_2.Triangle(algo=smeshBuilder.BLSURF)
params = algo2d.Parameters()
params.SetGeometricMesh( 1 )
params.SetAngleMesh( 4 )
params.SetPhySize( 8 )
algo2d.SetGradation(1.05)
params.SetQuadAllowed( True )
params.SetAnisotropic(True)

ok = Mesh_2.Compute()

if not ok:
  raise Exception("Error when computing Mesh_2")

# Check the areas of Mesh_2
mini_2, maxi_2 = getMinMaxArea(Mesh_2)
mini_2_ref = 0.000408
maxi_2_ref = 10.1982

if abs(mini_2-mini_2_ref) > area_tolerance:
  raise Exception("Min area of Mesh_2 incorrect")

if abs(maxi_2-maxi_2_ref) > area_tolerance:
  raise Exception("Max area of Mesh_2 incorrect")

# Check the number of faces of Mesh_2
nb_faces_2 = Mesh_2.NbFaces()
nb_faces_2_ref = 121
if abs(nb_faces_2-nb_faces_2_ref) > nb_faces_tolerance:
  raise Exception("Number of faces of Mesh_2 incorrect")

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(True)


