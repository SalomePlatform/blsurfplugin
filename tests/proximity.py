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
import math
import SALOMEDS


geompy = geomBuilder.New(theStudy)

# Create a box
box = geompy.MakeBoxDXDYDZ(100, 100, 100)

# Create a sphere near the sides of the box
sphere_tmp = geompy.MakeSphereR(5)
sphere = geompy.MakeTranslation(sphere_tmp, 6, 6, 50)

part = geompy.MakePartition([box, sphere])
geompy.addToStudy( box, 'box' )
geompy.addToStudy( sphere, 'sphere' )
geompy.addToStudy( part, 'part' )

# Create the groups of faces
box_faces = geompy.GetShapesOnBox(box, part, geompy.ShapeType["FACE"], GEOM.ST_ON)
gr_box_faces = geompy.CreateGroup(part, geompy.ShapeType["FACE"])
geompy.UnionList(gr_box_faces, box_faces)
geompy.addToStudyInFather(part, gr_box_faces, "box_faces")

all_faces = geompy.SubShapeAll(part, geompy.ShapeType["FACE"])
gr_all_faces = geompy.CreateGroup(part, geompy.ShapeType["FACE"])
geompy.UnionList(gr_all_faces, all_faces)

gr_spheres_faces = geompy.CutGroups(gr_all_faces, gr_box_faces)
geompy.addToStudyInFather(part, gr_spheres_faces, "spheres_faces")

###
### SMESH component
###

import  SMESH, SALOMEDS
from salome.smesh import smeshBuilder

smesh = smeshBuilder.New(theStudy)
Mesh_1 = smesh.Mesh(part, "Mesh_part")
BLSURF = Mesh_1.Triangle(algo=smeshBuilder.BLSURF)
BLSURF_Parameters_1 = BLSURF.Parameters()
BLSURF_Parameters_1.SetGradation( 1.2 )
BLSURF_Parameters_1.SetGeometricMesh( 1 )
BLSURF_Parameters_1.SetPhySize( 20 )
#BLSURF_Parameters_1.SetMinSize( 0.1 )
BLSURF_Parameters_1.SetMaxSize( 20 )
BLSURF_Parameters_1.SetAngleMesh( 8 )

# Old way to set proximity (Mesh Gems 1.1)
#BLSURF_Parameters_1.SetOptionValue( 'proximity', '1' )
#BLSURF_Parameters_1.SetOptionValue( 'prox_ratio', '1.2' )
#BLSURF_Parameters_1.SetOptionValue( 'prox_nb_layer', '3' )

# New way to set proximity (Mesh Gems >= 1.3)
BLSURF_Parameters_1.SetOptionValue( "volume_gradation", "1.2" )

isDone = Mesh_1.Compute()
if not isDone:
    raise Exception("Compute mesh ended in error")

# Create the groups on the mesh
gr_mesh_box = Mesh_1.Group(gr_box_faces)
gr_mesh_spheres = Mesh_1.Group(gr_spheres_faces)

# Check the minimal area of the box faces to check the proximity
min_area, max_area = Mesh_1.GetMinMax(SMESH.FT_Area, gr_mesh_box)

print "min_area: ", min_area

if min_area > 1.5:
    raise Exception("Wrong minimal area on box. Proximity has not worked.")

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(True)
